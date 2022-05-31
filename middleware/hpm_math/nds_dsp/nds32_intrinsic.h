#ifndef _NDS32_INTRINSIC_H
#define _NDS32_INTRINSIC_H

#include <nds_intrinsic.h>

#if __riscv_xlen == 32
#define __nds32__msync_all __nds__msync_all

#define __nds32__cctl_l1d_wball_one_lvl __nds__cctl_l1d_wball_one_lvl
#define __nds32__cctl_l1d_wball_alvl __nds__cctl_l1d_wball_alvl
#define __nds32__cctl_l1d_invalall __nds__cctl_l1d_invalall

#define __nds32__mfsr __nds__mfsr
#define __nds32__mtsr __nds__mtsr

#define __nds32__rotr __nds__rotr
#define __nds32__wsbh __nds__wsbh

#define __nds32__add16 __nds__add16
#define __nds32__v_uadd16 __nds__v_uadd16
#define __nds32__v_sadd16 __nds__v_sadd16
#define __nds32__radd16 __nds__radd16
#define __nds32__v_radd16 __nds__v_radd16
#define __nds32__uradd16 __nds__uradd16
#define __nds32__v_uradd16 __nds__v_uradd16
#define __nds32__kadd16 __nds__kadd16
#define __nds32__v_kadd16 __nds__v_kadd16
#define __nds32__ukadd16 __nds__ukadd16
#define __nds32__v_ukadd16 __nds__v_ukadd16
#define __nds32__sub16 __nds__sub16
#define __nds32__v_usub16 __nds__v_usub16
#define __nds32__v_ssub16 __nds__v_ssub16
#define __nds32__rsub16 __nds__rsub16
#define __nds32__v_rsub16 __nds__v_rsub16
#define __nds32__ursub16 __nds__ursub16
#define __nds32__v_ursub16 __nds__v_ursub16
#define __nds32__ksub16 __nds__ksub16
#define __nds32__v_ksub16 __nds__v_ksub16
#define __nds32__uksub16 __nds__uksub16
#define __nds32__v_uksub16 __nds__v_uksub16
#define __nds32__cras16 __nds__cras16
#define __nds32__v_ucras16 __nds__v_ucras16
#define __nds32__v_scras16 __nds__v_scras16
#define __nds32__rcras16 __nds__rcras16
#define __nds32__v_rcras16 __nds__v_rcras16
#define __nds32__urcras16 __nds__urcras16
#define __nds32__v_urcras16 __nds__v_urcras16
#define __nds32__kcras16 __nds__kcras16
#define __nds32__v_kcras16 __nds__v_kcras16
#define __nds32__ukcras16 __nds__ukcras16
#define __nds32__v_ukcras16 __nds__v_ukcras16
#define __nds32__crsa16 __nds__crsa16
#define __nds32__v_ucrsa16 __nds__v_ucrsa16
#define __nds32__v_scrsa16 __nds__v_scrsa16
#define __nds32__rcrsa16 __nds__rcrsa16
#define __nds32__v_rcrsa16 __nds__v_rcrsa16
#define __nds32__urcrsa16 __nds__urcrsa16
#define __nds32__v_urcrsa16 __nds__v_urcrsa16
#define __nds32__kcrsa16 __nds__kcrsa16
#define __nds32__v_kcrsa16 __nds__v_kcrsa16
#define __nds32__ukcrsa16 __nds__ukcrsa16
#define __nds32__v_ukcrsa16 __nds__v_ukcrsa16
#define __nds32__add8 __nds__add8
#define __nds32__v_uadd8 __nds__v_uadd8
#define __nds32__v_sadd8 __nds__v_sadd8
#define __nds32__radd8 __nds__radd8
#define __nds32__v_radd8 __nds__v_radd8
#define __nds32__uradd8 __nds__uradd8
#define __nds32__v_uradd8 __nds__v_uradd8
#define __nds32__kadd8 __nds__kadd8
#define __nds32__v_kadd8 __nds__v_kadd8
#define __nds32__ukadd8 __nds__ukadd8
#define __nds32__v_ukadd8 __nds__v_ukadd8
#define __nds32__sub8 __nds__sub8
#define __nds32__v_usub8 __nds__v_usub8
#define __nds32__v_ssub8 __nds__v_ssub8
#define __nds32__rsub8 __nds__rsub8
#define __nds32__v_rsub8 __nds__v_rsub8
#define __nds32__ursub8 __nds__ursub8
#define __nds32__v_ursub8 __nds__v_ursub8
#define __nds32__ksub8 __nds__ksub8
#define __nds32__v_ksub8 __nds__v_ksub8
#define __nds32__uksub8 __nds__uksub8
#define __nds32__v_uksub8 __nds__v_uksub8
#define __nds32__sra16 __nds__sra16
#define __nds32__v_sra16 __nds__v_sra16
#define __nds32__sra16_u __nds__sra16_u
#define __nds32__v_sra16_u __nds__v_sra16_u
#define __nds32__srl16 __nds__srl16
#define __nds32__v_srl16 __nds__v_srl16
#define __nds32__srl16_u __nds__srl16_u
#define __nds32__v_srl16_u __nds__v_srl16_u
#define __nds32__sll16 __nds__sll16
#define __nds32__v_sll16 __nds__v_sll16
#define __nds32__ksll16 __nds__ksll16
#define __nds32__v_ksll16 __nds__v_ksll16
#define __nds32__kslra16 __nds__kslra16
#define __nds32__v_kslra16 __nds__v_kslra16
#define __nds32__kslra16_u __nds__kslra16_u
#define __nds32__v_kslra16_u __nds__v_kslra16_u
#define __nds32__cmpeq16 __nds__cmpeq16
#define __nds32__v_scmpeq16 __nds__v_scmpeq16
#define __nds32__v_ucmpeq16 __nds__v_ucmpeq16
#define __nds32__scmplt16 __nds__scmplt16
#define __nds32__v_scmplt16 __nds__v_scmplt16
#define __nds32__scmple16 __nds__scmple16
#define __nds32__v_scmple16 __nds__v_scmple16
#define __nds32__ucmplt16 __nds__ucmplt16
#define __nds32__v_ucmplt16 __nds__v_ucmplt16
#define __nds32__ucmple16 __nds__ucmple16
#define __nds32__v_ucmple16 __nds__v_ucmple16
#define __nds32__cmpeq8 __nds__cmpeq8
#define __nds32__v_scmpeq8 __nds__v_scmpeq8
#define __nds32__v_ucmpeq8 __nds__v_ucmpeq8
#define __nds32__scmplt8 __nds__scmplt8
#define __nds32__v_scmplt8 __nds__v_scmplt8
#define __nds32__scmple8 __nds__scmple8
#define __nds32__v_scmple8 __nds__v_scmple8
#define __nds32__ucmplt8 __nds__ucmplt8
#define __nds32__v_ucmplt8 __nds__v_ucmplt8
#define __nds32__ucmple8 __nds__ucmple8
#define __nds32__v_ucmple8 __nds__v_ucmple8
#define __nds32__smin16 __nds__smin16
#define __nds32__v_smin16 __nds__v_smin16
#define __nds32__umin16 __nds__umin16
#define __nds32__v_umin16 __nds__v_umin16
#define __nds32__smax16 __nds__smax16
#define __nds32__v_smax16 __nds__v_smax16
#define __nds32__umax16 __nds__umax16
#define __nds32__v_umax16 __nds__v_umax16
#define __nds32__sclip16 __nds__sclip16
#define __nds32__v_sclip16 __nds__v_sclip16
#define __nds32__uclip16 __nds__uclip16
#define __nds32__v_uclip16 __nds__v_uclip16
#define __nds32__khm16 __nds__khm16
#define __nds32__v_khm16 __nds__v_khm16
#define __nds32__khmx16 __nds__khmx16
#define __nds32__v_khmx16 __nds__v_khmx16
#define __nds32__khm8 __nds__khm8
#define __nds32__v_khm8 __nds__v_khm8
#define __nds32__khmx8 __nds__khmx8
#define __nds32__v_khmx8 __nds__v_khmx8
#define __nds32__kabs16 __nds__kabs16
#define __nds32__v_kabs16 __nds__v_kabs16
#define __nds32__smul16 __nds__smul16
#define __nds32__v_smul16 __nds__v_smul16
#define __nds32__smulx16 __nds__smulx16
#define __nds32__v_smulx16 __nds__v_smulx16
#define __nds32__umul16 __nds__umul16
#define __nds32__v_umul16 __nds__v_umul16
#define __nds32__umulx16 __nds__umulx16
#define __nds32__v_umulx16 __nds__v_umulx16
#define __nds32__smul8 __nds__smul8
#define __nds32__v_smul8 __nds__v_smul8
#define __nds32__smulx8 __nds__smulx8
#define __nds32__v_smulx8 __nds__v_smulx8
#define __nds32__umul8 __nds__umul8
#define __nds32__v_umul8 __nds__v_umul8
#define __nds32__umulx8 __nds__umulx8
#define __nds32__v_umulx8 __nds__v_umulx8
#define __nds32__smin8 __nds__smin8
#define __nds32__v_smin8 __nds__v_smin8
#define __nds32__umin8 __nds__umin8
#define __nds32__v_umin8 __nds__v_umin8
#define __nds32__smax8 __nds__smax8
#define __nds32__v_smax8 __nds__v_smax8
#define __nds32__umax8 __nds__umax8
#define __nds32__v_umax8 __nds__v_umax8
#define __nds32__kabs8 __nds__kabs8
#define __nds32__v_kabs8 __nds__v_kabs8
#define __nds32__sunpkd810 __nds__sunpkd810
#define __nds32__v_sunpkd810 __nds__v_sunpkd810
#define __nds32__sunpkd820 __nds__sunpkd820
#define __nds32__v_sunpkd820 __nds__v_sunpkd820
#define __nds32__sunpkd830 __nds__sunpkd830
#define __nds32__v_sunpkd830 __nds__v_sunpkd830
#define __nds32__sunpkd831 __nds__sunpkd831
#define __nds32__v_sunpkd831 __nds__v_sunpkd831
#define __nds32__sunpkd832 __nds__sunpkd832
#define __nds32__v_sunpkd832 __nds__v_sunpkd832
#define __nds32__zunpkd810 __nds__zunpkd810
#define __nds32__v_zunpkd810 __nds__v_zunpkd810
#define __nds32__zunpkd820 __nds__zunpkd820
#define __nds32__v_zunpkd820 __nds__v_zunpkd820
#define __nds32__zunpkd830 __nds__zunpkd830
#define __nds32__v_zunpkd830 __nds__v_zunpkd830
#define __nds32__zunpkd831 __nds__zunpkd831
#define __nds32__v_zunpkd831 __nds__v_zunpkd831
#define __nds32__zunpkd832 __nds__zunpkd832
#define __nds32__v_zunpkd832 __nds__v_zunpkd832
#define __nds32__raddw __nds__raddw
#define __nds32__uraddw __nds__uraddw
#define __nds32__rsubw __nds__rsubw
#define __nds32__ursubw __nds__ursubw
#define __nds32__sra_u __nds__sra_u
#define __nds32__ksll __nds__ksllw
#define __nds32__pkbb16 __nds__pkbb16
#define __nds32__v_pkbb16 __nds__v_pkbb16
#define __nds32__pkbt16 __nds__pkbt16
#define __nds32__v_pkbt16 __nds__v_pkbt16
#define __nds32__pktb16 __nds__pktb16
#define __nds32__v_pktb16 __nds__v_pktb16
#define __nds32__pktt16 __nds__pktt16
#define __nds32__v_pktt16 __nds__v_pktt16
#define __nds32__smmul __nds__smmul
#define __nds32__smmul_u __nds__smmul_u
#define __nds32__kmmac __nds__kmmac
#define __nds32__kmmac_u __nds__kmmac_u
#define __nds32__kmmsb __nds__kmmsb
#define __nds32__kmmsb_u __nds__kmmsb_u
#define __nds32__kwmmul __nds__kwmmul
#define __nds32__kwmmul_u __nds__kwmmul_u
#define __nds32__smmwb __nds__smmwb
#define __nds32__v_smmwb __nds__v_smmwb
#define __nds32__smmwb_u __nds__smmwb_u
#define __nds32__v_smmwb_u __nds__v_smmwb_u
#define __nds32__smmwt __nds__smmwt
#define __nds32__v_smmwt __nds__v_smmwt
#define __nds32__smmwt_u __nds__smmwt_u
#define __nds32__v_smmwt_u __nds__v_smmwt_u
#define __nds32__kmmawb __nds__kmmawb
#define __nds32__v_kmmawb __nds__v_kmmawb
#define __nds32__kmmawb_u __nds__kmmawb_u
#define __nds32__v_kmmawb_u __nds__v_kmmawb_u
#define __nds32__kmmawt __nds__kmmawt
#define __nds32__v_kmmawt __nds__v_kmmawt
#define __nds32__kmmawt_u __nds__kmmawt_u
#define __nds32__v_kmmawt_u __nds__v_kmmawt_u
#define __nds32__smbb __nds__smbb16
#define __nds32__v_smbb __nds__v_smbb16
#define __nds32__smbt __nds__smbt16
#define __nds32__v_smbt __nds__v_smbt16
#define __nds32__smtt __nds__smtt
#define __nds32__v_smtt __nds__v_smtt16
#define __nds32__kmda __nds__kmda
#define __nds32__v_kmda __nds__v_kmda
#define __nds32__kmxda __nds__kmxda
#define __nds32__v_kmxda __nds__v_kmxda
#define __nds32__smds __nds__smds
#define __nds32__v_smds __nds__v_smds
#define __nds32__smdrs __nds__smdrs
#define __nds32__v_smdrs __nds__v_smdrs
#define __nds32__smxds __nds__smxds
#define __nds32__v_smxds __nds__v_smxds
#define __nds32__kmabb __nds__kmabb
#define __nds32__v_kmabb __nds__v_kmabb
#define __nds32__kmabt __nds__kmabt
#define __nds32__v_kmabt __nds__v_kmabt
#define __nds32__kmatt __nds__kmatt
#define __nds32__v_kmatt __nds__v_kmatt
#define __nds32__kmada __nds__kmada
#define __nds32__v_kmada __nds__v_kmada
#define __nds32__kmaxda __nds__kmaxda
#define __nds32__v_kmaxda __nds__v_kmaxda
#define __nds32__kmads __nds__kmads
#define __nds32__v_kmads __nds__v_kmads
#define __nds32__kmadrs __nds__kmadrs
#define __nds32__v_kmadrs __nds__v_kmadrs
#define __nds32__kmaxds __nds__kmaxds
#define __nds32__v_kmaxds __nds__v_kmaxds
#define __nds32__kmsda __nds__kmsda
#define __nds32__v_kmsda __nds__v_kmsda
#define __nds32__kmsxda __nds__kmsxda
#define __nds32__v_kmsxda __nds__v_kmsxda
#define __nds32__smal __nds__smal
#define __nds32__v_smal __nds__v_smal
#define __nds32__bitrev __nds__bitrev
#define __nds32__wext __nds__wext
#define __nds32__bpick __nds__bpick
#define __nds32__insb __nds__insb
#define __nds32__sadd64 __nds__sadd64
#define __nds32__uadd64 __nds__uadd64
#define __nds32__radd64 __nds__radd64
#define __nds32__uradd64 __nds__uradd64
#define __nds32__kadd64 __nds__kadd64
#define __nds32__ukadd64 __nds__ukadd64
#define __nds32__ssub64 __nds__ssub64
#define __nds32__usub64 __nds__usub64
#define __nds32__rsub64 __nds__rsub64
#define __nds32__ursub64 __nds__ursub64
#define __nds32__ksub64 __nds__ksub64
#define __nds32__uksub64 __nds__uksub64
#define __nds32__smar64 __nds__smar64
#define __nds32__smsr64 __nds__smsr64
#define __nds32__umar64 __nds__umar64
#define __nds32__umsr64 __nds__umsr64
#define __nds32__kmar64 __nds__kmar64
#define __nds32__kmsr64 __nds__kmsr64
#define __nds32__ukmar64 __nds__ukmar64
#define __nds32__ukmsr64 __nds__ukmsr64
#define __nds32__smalbb __nds__smalbb
#define __nds32__v_smalbb __nds__v_smalbb
#define __nds32__smalbt __nds__smalbt
#define __nds32__v_smalbt __nds__v_smalbt
#define __nds32__smaltt __nds__smaltt
#define __nds32__v_smaltt __nds__v_smaltt
#define __nds32__smalda __nds__smalda
#define __nds32__v_smalda __nds__v_smalda
#define __nds32__smalxda __nds__smalxda
#define __nds32__v_smalxda __nds__v_smalxda
#define __nds32__smalds __nds__smalds
#define __nds32__v_smalds __nds__v_smalds
#define __nds32__smaldrs __nds__smaldrs
#define __nds32__v_smaldrs __nds__v_smaldrs
#define __nds32__smalxds __nds__smalxds
#define __nds32__v_smalxds __nds__v_smalxds
#define __nds32__smslda __nds__smslda
#define __nds32__v_smslda __nds__v_smslda
#define __nds32__smslxda __nds__smslxda
#define __nds32__v_smslxda __nds__v_smslxda
#define __nds32__clip __nds__uclip32
#define __nds32__clips __nds__sclip32
#define __nds32__kabs __nds__kabsw
#define __nds32__kaddw __nds__kaddw
#define __nds32__kaddh __nds__kaddh
#define __nds32__ksubw __nds__ksubw
#define __nds32__ksubh __nds__ksubh
#define __nds32__ukaddw __nds__ukaddw
#define __nds32__ukaddh __nds__ukaddh
#define __nds32__uksubw __nds__uksubw
#define __nds32__uksubh __nds__uksubh
#define __nds32__kdmbb __nds__kdmbb
#define __nds32__v_kdmbb __nds__v_kdmbb
#define __nds32__kdmbt __nds__kdmbt
#define __nds32__v_kdmbt __nds__v_kdmbt
#define __nds32__kdmtt __nds__kdmtt
#define __nds32__v_kdmtt __nds__v_kdmtt
#define __nds32__khmbb __nds__khmbb
#define __nds32__v_khmbb __nds__v_khmbb
#define __nds32__khmbt __nds__khmbt
#define __nds32__v_khmbt __nds__v_khmbt
#define __nds32__khmtt __nds__khmtt
#define __nds32__v_khmtt __nds__v_khmtt
#define __nds32__kslraw __nds__kslraw
#define __nds32__kslraw_u __nds__kslraw_u
#define __nds32__ave __nds__ave
#define __nds32__max __nds__maxw
#define __nds32__min __nds__minw
#define __nds32__sra8 __nds__sra8
#define __nds32__v_sra8 __nds__v_sra8
#define __nds32__sra8_u __nds__sra8_u
#define __nds32__v_sra8_u __nds__v_sra8_u
#define __nds32__srl8 __nds__srl8
#define __nds32__v_srl8 __nds__v_srl8
#define __nds32__srl8_u __nds__srl8_u
#define __nds32__v_srl8_u __nds__v_srl8_u
#define __nds32__sll8 __nds__sll8
#define __nds32__v_sll8 __nds__v_sll8
#define __nds32__ksll8 __nds__ksll8
#define __nds32__v_ksll8 __nds__v_ksll8
#define __nds32__kslra8 __nds__kslra8
#define __nds32__v_kslra8 __nds__v_kslra8
#define __nds32__kslra8_u __nds__kslra8_u
#define __nds32__v_kslra8_u __nds__v_kslra8_u
#define __nds32__clz __nds__clz32
#define __nds32__clo __nds__clo32
#define __nds32__pbsad __nds__pbsad
#define __nds32__pbsada __nds__pbsada
#define __nds32__swap8 __nds__swap8
#define __nds32__v_swap8 __nds__v_swap8
#define __nds32__swap16 __nds__swap16
#define __nds32__v_swap16 __nds__v_swap16
#define __nds32__sclip8 __nds__sclip8
#define __nds32__v_sclip8 __nds__v_sclip8
#define __nds32__uclip8 __nds__uclip8
#define __nds32__v_uclip8 __nds__v_uclip8
#define __nds32__ffb __nds__ffb
#define __nds32__ffmism __nds__ffmism
#define __nds32__flmism __nds__flmism
#define __nds32__rdov __nds__rdov
#define __nds32__clrov __nds__clrov
#endif

#endif