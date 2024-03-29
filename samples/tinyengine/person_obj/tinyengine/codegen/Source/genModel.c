/* Automatically generated source file */
#include <float.h>
#include <tinyengine_function.h>
#include <tinyengine_function_fp.h>

#include "genNN.h"
#include "genModel.h"
#include "genInclude.h"

/* Variables used by all ops */
ADD_params add_params;
int i;
int8_t *int8ptr,*int8ptr2;
int32_t *int32ptr;
float *fptr,*fptr2,*fptr3;

signed char* getInput() {
    return &buffer0[81920];
}
signed char* getOutput() {
    return NNoutput;
}
const int anchors[3][3][2] = {
{{116,90},{156,198},{373,326},},
{{30,61},{62,45},{59,119},},
{{10,13},{16,30},{33,23},},
};
void det_post_procesing(int* box_cnt, det_box** ret_box, float threshold){
    int8_t *input_data[3]={&buffer0[0],&buffer0[15360],&buffer0[7680],};
    int8_t y_zero[3]={43,49,35,};
    float y_scale[3]={0.11086384952068329,0.12771575152873993,0.1385749727487564,};
 	postprocessing(input_data, y_zero, y_scale, &buffer[16800], 160, 128, 18, 1, anchors, 3, 0.45f, threshold, box_cnt, ret_box);
}
void end2endinference(q7_t* img){
    invoke(NULL);
}
void invoke(float* labels){
/* layer 0:CONV_2D */
convolve_s8_kernel3_inputch3_stride2_pad1_fpreq(&buffer0[81920],160,128,3,(const q7_t*) weight0,bias0,scales0,-128,18,-128,127,&buffer0[0],80,64,16,sbuf,kbuf,-18);
/* layer 1:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],80,64,16,(const q7_t*) CHWweight1,offsetBias1,offsetRBias1,scales1,-128,128,-128,127,&buffer0[0],80,64,16,sbuf,-128);
/* layer 2:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[0],80,64,16,(const q7_t*) weight2,bias2,scales2,-5,128,-128,127,&buffer0[163840],80,64,16,sbuf);
/* layer 3:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[163840],80,64,16,(const q7_t*) weight3,bias3,scales3,-128,5,-128,127,&buffer0[0],80,64,32,sbuf);
/* layer 4:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],80,64,32,(const q7_t*) CHWweight4,offsetBias4,offsetRBias4,scales4,-128,128,-128,127,&buffer0[0],40,32,32,sbuf,-128);
/* layer 5:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],40,32,32,(const q7_t*) weight5,bias5,scales5,7,128,-128,127,&buffer0[81920],40,32,16,sbuf);
/* layer 6:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[81920],40,32,16,(const q7_t*) weight6,bias6,scales6,-128,-7,-128,127,&buffer0[0],40,32,64,sbuf);
/* layer 7:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],40,32,64,(const q7_t*) CHWweight7,offsetBias7,offsetRBias7,scales7,-128,128,-128,127,&buffer0[0],20,16,64,sbuf,-128);
/* layer 8:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,64,(const q7_t*) weight8,bias8,scales8,6,128,-128,127,&buffer0[20480],20,16,16,sbuf);
/* layer 9:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[20480],20,16,16,(const q7_t*) weight9,bias9,scales9,-128,-6,-128,127,&buffer0[0],20,16,48,sbuf);
/* layer 10:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,48,(const q7_t*) CHWweight10,offsetBias10,offsetRBias10,scales10,-128,128,-128,127,&buffer0[0],20,16,48,sbuf,-128);
/* layer 11:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[0],20,16,48,(const q7_t*) weight11,bias11,scales11,2,128,-128,127,&buffer0[15360],20,16,16,sbuf);
/* layer 12:ADD */
add_fpreq(5120, &buffer0[15360],0.038596779108047485,2,&buffer0[20480],0.053349919617176056,6,0.05863343924283981,11,&buffer0[25600]);
/* layer 13:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[25600],20,16,16,(const q7_t*) weight12,bias12,scales12,-128,-11,-128,127,&buffer0[0],20,16,48,sbuf);
/* layer 14:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],20,16,48,(const q7_t*) CHWweight13,offsetBias13,offsetRBias13,scales13,-128,128,-128,127,&buffer0[0],10,8,48,sbuf,-128);
/* layer 15:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[0],10,8,48,(const q7_t*) weight14,bias14,scales14,1,128,-128,127,&buffer0[15360],10,8,32,sbuf);
/* layer 16:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[15360],10,8,32,(const q7_t*) weight15,bias15,scales15,-128,-1,-128,127,&buffer0[0],10,8,192,sbuf);
/* layer 17:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,192,(const q7_t*) CHWweight16,offsetBias16,offsetRBias16,scales16,-128,128,-128,127,&buffer0[0],10,8,192,sbuf,-128);
/* layer 18:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,192,(const q7_t*) weight17,bias17,scales17,6,128,-128,127,&buffer0[15360],10,8,32,sbuf);
/* layer 19:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[15360],10,8,32,(const q7_t*) weight18,bias18,scales18,-128,-6,-128,127,&buffer0[0],10,8,128,sbuf);
/* layer 20:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],10,8,128,(const q7_t*) CHWweight19,offsetBias19,offsetRBias19,scales19,-128,128,-128,127,&buffer0[0],5,4,128,sbuf,-128);
/* layer 21:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,128,(const q7_t*) weight20,bias20,scales20,2,128,-128,127,&buffer0[3840],5,4,48,sbuf);
/* layer 22:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[3840],5,4,48,(const q7_t*) weight21,bias21,scales21,-128,-2,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 23:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight22,offsetBias22,offsetRBias22,scales22,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 24:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight23,bias23,scales23,3,128,-128,127,&buffer0[4800],5,4,48,sbuf);
/* layer 25:ADD */
add_fpreq(960, &buffer0[4800],0.05103969946503639,3,&buffer0[3840],0.0475192666053772,2,0.06983265280723572,0,&buffer0[5760]);
/* layer 26:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[5760],5,4,48,(const q7_t*) weight24,bias24,scales24,-128,0,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 27:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight25,offsetBias25,offsetRBias25,scales25,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 28:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight26,bias26,scales26,16,128,-128,127,&buffer0[3840],5,4,48,sbuf);
/* layer 29:ADD */
add_fpreq(960, &buffer0[3840],0.13467691838741302,16,&buffer0[5760],0.06983265280723572,0,0.15181373059749603,11,&buffer0[38400]);
/* layer 30:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[38400],5,4,48,(const q7_t*) weight27,bias27,scales27,-128,-11,-128,127,&buffer0[0],5,4,32,sbuf);
/* layer 31:UPSAMPLE */
upsample_byte(&buffer0[0], 4, 5, 32, &buffer0[640], 2);
/* layer 32:ADD */
add_fpreq(2560, &buffer0[640],0.023517923429608345,-128,&buffer0[15360],0.05543658882379532,6,0.06994528323411942,-32,&buffer0[7680]);
/* layer 33:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[7680],10.0,8.0,32,(const q7_t*) weight28,bias28,scales28,-128,32,-128,127,&buffer0[0],10,8,96,sbuf);
/* layer 34:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,96,(const q7_t*) CHWweight29,offsetBias29,offsetRBias29,scales29,-128,128,-128,127,&buffer0[0],10,8,96,sbuf,-128);
/* layer 35:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,96,(const q7_t*) weight30,bias30,scales30,10,128,-128,127,&buffer0[10240],10,8,32,sbuf);
/* layer 36:ADD */
add_fpreq(2560, &buffer0[10240],0.1363428384065628,10,&buffer0[7680],0.06994528323411942,-32,0.1592041552066803,2,&buffer0[30720]);
/* layer 37:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[30720],10,8,32,(const q7_t*) weight31,bias31,scales31,-128,-2,-128,127,&buffer0[5120],10,8,16,sbuf);
/* layer 38:UPSAMPLE */
upsample_byte(&buffer0[5120], 8, 10, 16, &buffer0[0], 2);
/* layer 39:ADD */
add_fpreq(5120, &buffer0[0],0.023517923429608345,-128,&buffer0[25600],0.05863343924283981,11,0.07239335775375366,-24,&buffer0[10240]);
/* layer 40:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[10240],20.0,16.0,16,(const q7_t*) weight32,bias32,scales32,-128,24,-128,127,&buffer0[0],20,16,32,sbuf);
/* layer 41:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,32,(const q7_t*) CHWweight33,offsetBias33,offsetRBias33,scales33,-128,128,-128,127,&buffer0[0],20,16,32,sbuf,-128);
/* layer 42:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,32,(const q7_t*) weight34,bias34,scales34,-7,128,-128,127,&buffer0[15360],20,16,16,sbuf);
/* layer 43:ADD */
add_fpreq(5120, &buffer0[15360],0.1112816333770752,-7,&buffer0[10240],0.07239335775375366,-24,0.13439886271953583,-22,&buffer0[20480]);
/* layer 44:MAX_POOL_2D */
max_pooling(&buffer0[20480],16,20,16,2,2,8,10,-128,127,&buffer0[25600]);
/* layer 45:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[20480],20,16,16,(const q7_t*) weight35,bias35,scales35,-128,22,-128,127,&buffer0[0],20,16,64,sbuf);
/* layer 46:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[25600],10,8,16,(const q7_t*) weight36,bias36,scales36,-128,22,-128,127,&buffer0[20480],10,8,32,sbuf);
/* layer 47:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,64,(const q7_t*) CHWweight37,offsetBias37,offsetRBias37,scales37,-128,128,-128,127,&buffer0[0],20,16,64,sbuf,-128);
/* layer 48:ADD */
add_fpreq(2560, &buffer0[20480],0.023517923429608345,-128,&buffer0[30720],0.1592041552066803,2,0.17074668407440186,-8,&buffer0[35840]);
/* layer 49:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,64,(const q7_t*) weight38,bias38,scales38,-128,128,-128,127,&buffer0[20480],20,16,48,sbuf);
/* layer 50:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[35840],10,8,32,(const q7_t*) weight39,bias39,scales39,-128,8,-128,127,&buffer0[0],10,8,64,sbuf);
/* layer 51:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[20480],20,16,48,(const q7_t*) weight40,bias40,scales40,35,128,-128,127,&buffer0[7680],20,16,18,sbuf);
/* layer 52:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,64,(const q7_t*) CHWweight41,offsetBias41,offsetRBias41,scales41,-128,128,-128,127,&buffer0[0],10,8,64,sbuf,-128);
/* layer 53:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,64,(const q7_t*) weight42,bias42,scales42,-10,128,-128,127,&buffer0[5120],10,8,32,sbuf);
/* layer 54:ADD */
add_fpreq(2560, &buffer0[5120],0.24770784378051758,-10,&buffer0[35840],0.17074668407440186,-8,0.2864508628845215,-17,&buffer0[0]);
/* layer 55:MAX_POOL_2D */
max_pooling(&buffer0[0],8,10,32,2,2,4,5,-128,127,&buffer0[2560]);
/* layer 56:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,32,(const q7_t*) weight43,bias43,scales43,-128,17,-128,127,&buffer0[13440],10,8,64,sbuf);
/* layer 57:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[2560],5,4,32,(const q7_t*) weight44,bias44,scales44,-128,17,-128,127,&buffer0[0],5,4,48,sbuf);
/* layer 58:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[13440],10,8,64,(const q7_t*) CHWweight45,offsetBias45,offsetRBias45,scales45,-128,128,-128,127,&buffer0[13440],10,8,64,sbuf,-128);
/* layer 59:ADD */
add_fpreq(960, &buffer0[0],0.023517923429608345,-128,&buffer0[38400],0.15181373059749603,11,0.15291480720043182,0,&buffer0[18560]);
/* layer 60:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[13440],10,8,64,(const q7_t*) weight46,bias46,scales46,-128,128,-128,127,&buffer0[0],10,8,96,sbuf);
/* layer 61:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[18560],5,4,48,(const q7_t*) weight47,bias47,scales47,-128,0,-128,127,&buffer0[13440],5,4,96,sbuf);
/* layer 62:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,96,(const q7_t*) weight48,bias48,scales48,49,128,-128,127,&buffer0[15360],10,8,18,sbuf);
/* layer 63:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[13440],5,4,96,(const q7_t*) CHWweight49,offsetBias49,offsetRBias49,scales49,-128,128,-128,127,&buffer0[13440],5,4,96,sbuf,-128);
/* layer 64:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[13440],5,4,96,(const q7_t*) weight50,bias50,scales50,1,128,-128,127,&buffer0[0],5,4,48,sbuf);
/* layer 65:ADD */
add_fpreq(960, &buffer0[0],0.15737907588481903,1,&buffer0[18560],0.15291480720043182,0,0.20549826323986053,-4,&buffer0[3840]);
/* layer 66:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[3840],5,4,48,(const q7_t*) weight51,bias51,scales51,-128,4,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 67:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight52,offsetBias52,offsetRBias52,scales52,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 68:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight53,bias53,scales53,-128,128,-128,127,&buffer0[3840],5,4,144,sbuf);
/* layer 69:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[3840],5,4,144,(const q7_t*) weight54,bias54,scales54,43,128,-128,127,&buffer0[0],5,4,18,sbuf);
}
void invoke_inf(){
/* layer 0:CONV_2D */
convolve_s8_kernel3_inputch3_stride2_pad1_fpreq(&buffer0[81920],160,128,3,(const q7_t*) weight0,bias0,scales0,-128,18,-128,127,&buffer0[0],80,64,16,sbuf,kbuf,-18);
/* layer 1:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],80,64,16,(const q7_t*) CHWweight1,offsetBias1,offsetRBias1,scales1,-128,128,-128,127,&buffer0[0],80,64,16,sbuf,-128);
/* layer 2:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[0],80,64,16,(const q7_t*) weight2,bias2,scales2,-5,128,-128,127,&buffer0[163840],80,64,16,sbuf);
/* layer 3:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[163840],80,64,16,(const q7_t*) weight3,bias3,scales3,-128,5,-128,127,&buffer0[0],80,64,32,sbuf);
/* layer 4:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],80,64,32,(const q7_t*) CHWweight4,offsetBias4,offsetRBias4,scales4,-128,128,-128,127,&buffer0[0],40,32,32,sbuf,-128);
/* layer 5:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],40,32,32,(const q7_t*) weight5,bias5,scales5,7,128,-128,127,&buffer0[81920],40,32,16,sbuf);
/* layer 6:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[81920],40,32,16,(const q7_t*) weight6,bias6,scales6,-128,-7,-128,127,&buffer0[0],40,32,64,sbuf);
/* layer 7:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],40,32,64,(const q7_t*) CHWweight7,offsetBias7,offsetRBias7,scales7,-128,128,-128,127,&buffer0[0],20,16,64,sbuf,-128);
/* layer 8:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,64,(const q7_t*) weight8,bias8,scales8,6,128,-128,127,&buffer0[20480],20,16,16,sbuf);
/* layer 9:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[20480],20,16,16,(const q7_t*) weight9,bias9,scales9,-128,-6,-128,127,&buffer0[0],20,16,48,sbuf);
/* layer 10:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,48,(const q7_t*) CHWweight10,offsetBias10,offsetRBias10,scales10,-128,128,-128,127,&buffer0[0],20,16,48,sbuf,-128);
/* layer 11:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[0],20,16,48,(const q7_t*) weight11,bias11,scales11,2,128,-128,127,&buffer0[15360],20,16,16,sbuf);
/* layer 12:ADD */
add_fpreq(5120, &buffer0[15360],0.038596779108047485,2,&buffer0[20480],0.053349919617176056,6,0.05863343924283981,11,&buffer0[25600]);
/* layer 13:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[25600],20,16,16,(const q7_t*) weight12,bias12,scales12,-128,-11,-128,127,&buffer0[0],20,16,48,sbuf);
/* layer 14:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],20,16,48,(const q7_t*) CHWweight13,offsetBias13,offsetRBias13,scales13,-128,128,-128,127,&buffer0[0],10,8,48,sbuf,-128);
/* layer 15:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[0],10,8,48,(const q7_t*) weight14,bias14,scales14,1,128,-128,127,&buffer0[15360],10,8,32,sbuf);
/* layer 16:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[15360],10,8,32,(const q7_t*) weight15,bias15,scales15,-128,-1,-128,127,&buffer0[0],10,8,192,sbuf);
/* layer 17:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,192,(const q7_t*) CHWweight16,offsetBias16,offsetRBias16,scales16,-128,128,-128,127,&buffer0[0],10,8,192,sbuf,-128);
/* layer 18:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,192,(const q7_t*) weight17,bias17,scales17,6,128,-128,127,&buffer0[15360],10,8,32,sbuf);
/* layer 19:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[15360],10,8,32,(const q7_t*) weight18,bias18,scales18,-128,-6,-128,127,&buffer0[0],10,8,128,sbuf);
/* layer 20:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride2_inplace_CHW_fpreq(&buffer0[0],10,8,128,(const q7_t*) CHWweight19,offsetBias19,offsetRBias19,scales19,-128,128,-128,127,&buffer0[0],5,4,128,sbuf,-128);
/* layer 21:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,128,(const q7_t*) weight20,bias20,scales20,2,128,-128,127,&buffer0[3840],5,4,48,sbuf);
/* layer 22:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[3840],5,4,48,(const q7_t*) weight21,bias21,scales21,-128,-2,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 23:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight22,offsetBias22,offsetRBias22,scales22,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 24:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight23,bias23,scales23,3,128,-128,127,&buffer0[4800],5,4,48,sbuf);
/* layer 25:ADD */
add_fpreq(960, &buffer0[4800],0.05103969946503639,3,&buffer0[3840],0.0475192666053772,2,0.06983265280723572,0,&buffer0[5760]);
/* layer 26:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[5760],5,4,48,(const q7_t*) weight24,bias24,scales24,-128,0,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 27:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight25,offsetBias25,offsetRBias25,scales25,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 28:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight26,bias26,scales26,16,128,-128,127,&buffer0[3840],5,4,48,sbuf);
/* layer 29:ADD */
add_fpreq(960, &buffer0[3840],0.13467691838741302,16,&buffer0[5760],0.06983265280723572,0,0.15181373059749603,11,&buffer0[38400]);
/* layer 30:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[38400],5,4,48,(const q7_t*) weight27,bias27,scales27,-128,-11,-128,127,&buffer0[0],5,4,32,sbuf);
/* layer 31:UPSAMPLE */
upsample_byte(&buffer0[0], 4, 5, 32, &buffer0[640], 2);
/* layer 32:ADD */
add_fpreq(2560, &buffer0[640],0.023517923429608345,-128,&buffer0[15360],0.05543658882379532,6,0.06994528323411942,-32,&buffer0[7680]);
/* layer 33:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[7680],10.0,8.0,32,(const q7_t*) weight28,bias28,scales28,-128,32,-128,127,&buffer0[0],10,8,96,sbuf);
/* layer 34:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,96,(const q7_t*) CHWweight29,offsetBias29,offsetRBias29,scales29,-128,128,-128,127,&buffer0[0],10,8,96,sbuf,-128);
/* layer 35:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,96,(const q7_t*) weight30,bias30,scales30,10,128,-128,127,&buffer0[10240],10,8,32,sbuf);
/* layer 36:ADD */
add_fpreq(2560, &buffer0[10240],0.1363428384065628,10,&buffer0[7680],0.06994528323411942,-32,0.1592041552066803,2,&buffer0[30720]);
/* layer 37:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[30720],10,8,32,(const q7_t*) weight31,bias31,scales31,-128,-2,-128,127,&buffer0[5120],10,8,16,sbuf);
/* layer 38:UPSAMPLE */
upsample_byte(&buffer0[5120], 8, 10, 16, &buffer0[0], 2);
/* layer 39:ADD */
add_fpreq(5120, &buffer0[0],0.023517923429608345,-128,&buffer0[25600],0.05863343924283981,11,0.07239335775375366,-24,&buffer0[10240]);
/* layer 40:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[10240],20.0,16.0,16,(const q7_t*) weight32,bias32,scales32,-128,24,-128,127,&buffer0[0],20,16,32,sbuf);
/* layer 41:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,32,(const q7_t*) CHWweight33,offsetBias33,offsetRBias33,scales33,-128,128,-128,127,&buffer0[0],20,16,32,sbuf,-128);
/* layer 42:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,32,(const q7_t*) weight34,bias34,scales34,-7,128,-128,127,&buffer0[15360],20,16,16,sbuf);
/* layer 43:ADD */
add_fpreq(5120, &buffer0[15360],0.1112816333770752,-7,&buffer0[10240],0.07239335775375366,-24,0.13439886271953583,-22,&buffer0[20480]);
/* layer 44:MAX_POOL_2D */
max_pooling(&buffer0[20480],16,20,16,2,2,8,10,-128,127,&buffer0[25600]);
/* layer 45:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[20480],20,16,16,(const q7_t*) weight35,bias35,scales35,-128,22,-128,127,&buffer0[0],20,16,64,sbuf);
/* layer 46:CONV_2D */
convolve_1x1_s8_ch16_fpreq(&buffer0[25600],10,8,16,(const q7_t*) weight36,bias36,scales36,-128,22,-128,127,&buffer0[20480],10,8,32,sbuf);
/* layer 47:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],20,16,64,(const q7_t*) CHWweight37,offsetBias37,offsetRBias37,scales37,-128,128,-128,127,&buffer0[0],20,16,64,sbuf,-128);
/* layer 48:ADD */
add_fpreq(2560, &buffer0[20480],0.023517923429608345,-128,&buffer0[30720],0.1592041552066803,2,0.17074668407440186,-8,&buffer0[35840]);
/* layer 49:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],20,16,64,(const q7_t*) weight38,bias38,scales38,-128,128,-128,127,&buffer0[20480],20,16,48,sbuf);
/* layer 50:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[35840],10,8,32,(const q7_t*) weight39,bias39,scales39,-128,8,-128,127,&buffer0[0],10,8,64,sbuf);
/* layer 51:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[20480],20,16,48,(const q7_t*) weight40,bias40,scales40,35,128,-128,127,&buffer0[7680],20,16,18,sbuf);
/* layer 52:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],10,8,64,(const q7_t*) CHWweight41,offsetBias41,offsetRBias41,scales41,-128,128,-128,127,&buffer0[0],10,8,64,sbuf,-128);
/* layer 53:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,64,(const q7_t*) weight42,bias42,scales42,-10,128,-128,127,&buffer0[5120],10,8,32,sbuf);
/* layer 54:ADD */
add_fpreq(2560, &buffer0[5120],0.24770784378051758,-10,&buffer0[35840],0.17074668407440186,-8,0.2864508628845215,-17,&buffer0[0]);
/* layer 55:MAX_POOL_2D */
max_pooling(&buffer0[0],8,10,32,2,2,4,5,-128,127,&buffer0[2560]);
/* layer 56:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,32,(const q7_t*) weight43,bias43,scales43,-128,17,-128,127,&buffer0[13440],10,8,64,sbuf);
/* layer 57:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[2560],5,4,32,(const q7_t*) weight44,bias44,scales44,-128,17,-128,127,&buffer0[0],5,4,48,sbuf);
/* layer 58:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[13440],10,8,64,(const q7_t*) CHWweight45,offsetBias45,offsetRBias45,scales45,-128,128,-128,127,&buffer0[13440],10,8,64,sbuf,-128);
/* layer 59:ADD */
add_fpreq(960, &buffer0[0],0.023517923429608345,-128,&buffer0[38400],0.15181373059749603,11,0.15291480720043182,0,&buffer0[18560]);
/* layer 60:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[13440],10,8,64,(const q7_t*) weight46,bias46,scales46,-128,128,-128,127,&buffer0[0],10,8,96,sbuf);
/* layer 61:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[18560],5,4,48,(const q7_t*) weight47,bias47,scales47,-128,0,-128,127,&buffer0[13440],5,4,96,sbuf);
/* layer 62:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],10,8,96,(const q7_t*) weight48,bias48,scales48,49,128,-128,127,&buffer0[15360],10,8,18,sbuf);
/* layer 63:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[13440],5,4,96,(const q7_t*) CHWweight49,offsetBias49,offsetRBias49,scales49,-128,128,-128,127,&buffer0[13440],5,4,96,sbuf,-128);
/* layer 64:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[13440],5,4,96,(const q7_t*) weight50,bias50,scales50,1,128,-128,127,&buffer0[0],5,4,48,sbuf);
/* layer 65:ADD */
add_fpreq(960, &buffer0[0],0.15737907588481903,1,&buffer0[18560],0.15291480720043182,0,0.20549826323986053,-4,&buffer0[3840]);
/* layer 66:CONV_2D */
convolve_1x1_s8_ch48_fpreq(&buffer0[3840],5,4,48,(const q7_t*) weight51,bias51,scales51,-128,4,-128,127,&buffer0[0],5,4,192,sbuf);
/* layer 67:DEPTHWISE_CONV_2D */
depthwise_kernel3x3_stride1_inplace_CHW_fpreq(&buffer0[0],5,4,192,(const q7_t*) CHWweight52,offsetBias52,offsetRBias52,scales52,-128,128,-128,127,&buffer0[0],5,4,192,sbuf,-128);
/* layer 68:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[0],5,4,192,(const q7_t*) weight53,bias53,scales53,-128,128,-128,127,&buffer0[3840],5,4,144,sbuf);
/* layer 69:CONV_2D */
convolve_1x1_s8_fpreq(&buffer0[3840],5,4,144,(const q7_t*) weight54,bias54,scales54,43,128,-128,127,&buffer0[0],5,4,18,sbuf);
}
