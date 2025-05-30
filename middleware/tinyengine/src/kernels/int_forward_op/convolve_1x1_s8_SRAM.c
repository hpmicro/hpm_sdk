/* ----------------------------------------------------------------------
 * Project: TinyEngine
 * Title:   convolve_1x1_s8_SRAM.c
 *
 * Reference papers:
 *  - MCUNet: Tiny Deep Learning on IoT Device, NeurIPS 2020
 *  - MCUNetV2: Memory-Efficient Patch-based Inference for Tiny Deep Learning, NeurIPS 2021
 *  - MCUNetV3: On-Device Training Under 256KB Memory, NeurIPS 2022
 * Contact authors:
 *  - Wei-Ming Chen, wmchen@mit.edu
 *  - Wei-Chen Wang, wweichen@mit.edu
 *  - Ji Lin, jilin@mit.edu
 *  - Ligeng Zhu, ligeng@mit.edu
 *  - Song Han, songhan@mit.edu
 *
 * Target ISA:  ARMv7E-M
 * -------------------------------------------------------------------- */
/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Target ISA:  RISCV D45
 *
 */

#include "hpm_math.h"
#include "tinyengine_function.h"
#include "img2col_element.h"
#include "kernel_element.h"

#define DIM_KER_X (1U)
#define DIM_KER_Y (1U)

//#define FULL_UNROLL

tinyengine_status convolve_1x1_s8_SRAM(const q7_t *input, const uint16_t input_x,
		const uint16_t input_y, const uint16_t input_ch, const q7_t *kernel,
		const int32_t *bias, const int32_t *output_shift,
		const int32_t *output_mult, const int32_t out_offset,
		const int32_t input_offset, const int32_t out_activation_min,
		const int32_t out_activation_max, q7_t *output, const uint16_t output_x,
		const uint16_t output_y, const uint16_t output_ch, q15_t *runtime_buf, q15_t *kbuf) {
	if (input_ch % 4 != 0 || input_ch % 2 != 0) {
		return PARAM_NO_SUPPORT;
	}

	int32_t i_element;
	(void) input_x;
	(void) input_y;

	/* Partial(two columns) im2col buffer */
	q15_t *two_column_buffer = runtime_buf;
	q7_t *out = output;
	const int32_t num_elements = output_x * output_y;
	const int channel_div4 = (input_ch >> 2);

	const int16_t inoff16 = input_offset;
	q31_t offset_q15x2 = __PKHBT(inoff16, inoff16, 16);

	/* whether kernels can fit in the buffer */
	//fill in kernels
	const q7_t *ip_a0 = kernel;
	for (int i = 0; i < output_ch; i += 2) {
		q31_t *dst1 = &kbuf[i * input_ch / 2]; //each q31_t store 2 elements
		q31_t *dst2 = dst1 + input_ch / 2;

		/* align the second pointer for A */
		const q7_t *ip_a1 = ip_a0 + input_ch;

		uint16_t col_count = input_ch / 4;
		/* accumulate over the vector */
		while (col_count) {
			q31_t a01, a02, a11, a12;

			ip_a0 = read_and_pad_reordered(ip_a0, &dst1[0], &dst1[1]);
			ip_a1 = read_and_pad_reordered(ip_a1, &dst2[0], &dst2[1]);

			dst1 += 2;
			dst2 += 2;
			col_count--;
		} /* while over col_count */

		/* skip row */
		ip_a0 += input_ch;
	}

	/* output stationary */
	for (i_element = 0; i_element < num_elements; i_element += 2) {
		q7_t *src = &input[i_element * input_ch];
		q15_t *dst = two_column_buffer;

		//use variables
		q31_t in_q7x4;
		q31_t in_q15x2_1;
		q31_t in_q15x2_2;
		q31_t out_q15x2_1;
		q31_t out_q15x2_2;

		int cnt = channel_div4; //two columns
		while (cnt > 0) {
			q7_q15_offset_reordered_ele(src, dst)
			q7_q15_offset_reordered_ele(src, dst)
			cnt--;
		}

		out = mat_mult_s16(kernel,
				two_column_buffer, output_ch, output_shift, output_mult,
				(q7_t) out_offset, out_activation_min,
				out_activation_max, input_ch,
				bias, out, kbuf);

	}

	/* check if there is an odd column left-over for computation */
	if (num_elements & 0x1) {
		int32_t i_ch_out;
		const q7_t *ker_a = kernel;
		q7_t *src = &input[(num_elements - 1) * input_ch];
		q15_t *dst = two_column_buffer;

		//use variables
		q31_t in_q7x4;
		q31_t in_q15x2_1;
		q31_t in_q15x2_2;
		q31_t out_q15x2_1;
		q31_t out_q15x2_2;

		int cnt = channel_div4;	//two * numof2col columns
		while (cnt > 0) {
			q7_q15_offset_reordered_ele(src, dst)
			cnt--;
		}

		for (i_ch_out = 0; i_ch_out < output_ch; i_ch_out++) {
			q31_t sum = bias[i_ch_out];

			/* Point to the beginning of the im2col buffer where the input is available as a rearranged column */
			const q15_t *ip_as_col = runtime_buf;
			uint16_t col_count = (input_ch * DIM_KER_X * DIM_KER_Y) >> 2;

			while (col_count) {
				q31_t ker_a1, ker_a2;
				q31_t in_b1, in_b2;
				ker_a = read_and_pad_reordered(ker_a, &ker_a1, &ker_a2);

				in_b1 = hpm_nn_read_q15x2_ia(&ip_as_col);
				sum = __SMLAD(ker_a1, in_b1, sum);
				in_b2 = hpm_nn_read_q15x2_ia(&ip_as_col);
				sum = __SMLAD(ker_a2, in_b2, sum);

				col_count--;
			}

			sum = hpm_nn_requantize(sum, output_mult[i_ch_out],
					output_shift[i_ch_out]);
			sum += out_offset;
			sum = MAX(sum, out_activation_min);
			sum = MIN(sum, out_activation_max);
			*out++ = (q7_t) sum;
		}
	}

	/* Return to application */
	return STATE_SUCCESS;
}
