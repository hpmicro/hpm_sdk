/*
 *  #AP6256_NVRAM_V1.4_06112021
 */

const char wifi_nvram_image[] = {
	"NVRAMRev=$Rev: 498373 $"											"\x00"
	"sromrev=11" 														"\x00"
	"vendid=0x14e4" 													"\x00"
	"devid=0x43ab" 														"\x00"
	"manfid=0x2d0" 														"\x00"
	"prodid=0x06e4" 													"\x00"
	"macaddr=00:90:4c:c5:12:38" 										"\x00"
	"nocrc=1" 															"\x00"
	"boardtype=0x6e4" 													"\x00"
	"boardrev=0x1304" 													"\x00"
	//#XTAL 37.4MHz
	"xtalfreq=37400" 													"\x00"
	"btc_mode=1" 														"\x00"
	//#-----------------------------------------
	//#boardflags: 5GHz eTR switch by default
	//#            2.4GHz eTR switch by default
	//#            bit1 for btcoex
	"boardflags=0x00480201" 											"\x00"
	"boardflags2=0x40800000" 											"\x00"
	"boardflags3=0x48200100" 											"\x00"
	"phycal_tempdelta=15" 												"\x00"
	"rxchain=1" 														"\x00"
	"txchain=1" 														"\x00"
	"aa2g=1" 															"\x00"
	"aa5g=1" 															"\x00"
	"tssipos5g=1" 														"\x00"
	"tssipos2g=1" 														"\x00"
	"femctrl=0" 														"\x00"
	"AvVmid_c0=1,165,2,100,2,100,2,100,2,100" 							"\x00"
	"pa2ga0=-154,5722,-656" 											"\x00"
	//#1pa2ga0=-195,5558,-672
	"pa2ga1=-118,4850,-635" 											"\x00"
	"pa5ga0=-193,6098,-740,-192,6095,-738,-199,5996,-730,-182,6148,-741" "\x00"
	//#3pa5ga0=-214,6002,-737,-209,6102,-746,-209,6024,-738,-200,6149,-747
	//#2pa5ga0=-203,5495,-683,-205,5489,-682,-207,5494,-678,-208,5549,-683
	//#1pa5ga0=-203,5495,-683,-205,5489,-682,-207,5494,-678,-185,6130,-740
	"pa5ga1=-142,4930,-655,-150,4859,-646,-156,4887,-644,-158,4864,-641" "\x00"
	"itrsw=1" 															"\x00"
	"pdoffset2g40ma0=10" 												"\x00"
	"pdoffset40ma0=0xaaaa" 												"\x00"
	"pdoffset80ma0=0xaaaa" 												"\x00"
	"extpagain5g=2" 													"\x00"
	"extpagain2g=2" 													"\x00"
	"tworangetssi2g=0" 													"\x00"
	"tworangetssi5g=0" 													"\x00"
	//# LTECX flags
	//# WCI2
	"ltecxmux=0" 														"\x00"
	"ltecxpadnum=0x0504" 												"\x00"
	"ltecxfnsel=0x22" 													"\x00"
	"ltecxgcigpio=0x32" 												"\x00"
	"maxp2ga0=78" 														"\x00"
	"cckbw202gpo=0x5555" 												"\x00"
	"cckbw20ul2gpo=0x5555" 												"\x00"
	"ofdmlrbw202gpo=0x2244" 											"\x00"
	"dot11agofdmhrbw202gpo=0x7744" 										"\x00"
	"ccode=DE" 															"\x00"
	"regrev=0" 															"\x00"
	"mcsbw202gpo=0x98886422" 											"\x00"
	"mcsbw402gpo=0x98777755" 											"\x00"
	"maxp5ga0=75,75,75,76" 												"\x00"
	//#5GHz_20MHz Offset
	"mcsbw205glpo=0xba753311" 											"\x00"
	"mcsbw205gmpo=0xba753311" 											"\x00"
	"mcsbw205ghpo=0xba753311" 											"\x00"
	//#5GHz_40MHz Offset
	"mcsbw405glpo=0xe8753311" 											"\x00"
	"mcsbw405gmpo=0xe8753311" 											"\x00"
	"mcsbw405ghpo=0xd9753311" 											"\x00"
	//#5GHz_80MHz Offset
	"mcsbw805glpo=0xea633311" 											"\x00"
	"mcsbw805gmpo=0xe7422211" 											"\x00"
	"mcsbw805ghpo=0xda632211" 											"\x00"
	"swctrlmap_2g=0x00000000,0x00000000,0x00000000,0x010000,0x3ff" 		"\x00"
	"swctrlmap_5g=0x00100010,0x00200020,0x00200020,0x010000,0x3fe" 		"\x00"
	"swctrlmapext_5g=0x00000000,0x00000000,0x00000000,0x000000,0x3" 	"\x00"
	"swctrlmapext_2g=0x00000000,0x00000000,0x00000000,0x000000,0x3" 	"\x00"
	"vcodivmode=1" 														"\x00"
	"deadman_to=481500000" 												"\x00"
	"ed_thresh2g=-54" 													"\x00"
	"ed_thresh5g=-54" 													"\x00"
	"eu_edthresh2g=-54" 												"\x00"
	"eu_edthresh5g=-54" 												"\x00"
	//#ed_thresh2g=-69
	//#ed_thresh5g=-69
	//#eu_edthresh2g=-69
	//#eu_edthresh5g=-69
	"ldo1=4" 															"\x00"
	"rawtempsense=0x1ff" 												"\x00"
	"cckPwrIdxCorr=3" 													"\x00"
	"cckTssiDelay=150" 													"\x00"
	"ofdmTssiDelay=150" 												"\x00"
	"txpwr2gAdcScale=1" 												"\x00"
	"txpwr5gAdcScale=1" 												"\x00"
	"dot11b_opts=0x3aa85" 												"\x00"
	"cbfilttype=0" 														"\x00"
	//#fdsslevel_ch1=6
	//#fdsslevel_ch11=4
	//#btc_mode=1  
	"muxenab=0x10" 														"\x00"
	//"muxenab=0x1f" 														"\x00"
	"cckdigfilttype=5" 													"\x00"
	"fdss_level_2g=6" 													"\x00"
	//#fdss_level_5g=6
	//#pacalshift5g=0,0,1
	"tempthresh=102" 													"\x00"
	"pa2ga0_tsmc=-154,5722,-656" 										"\x00"
	"pa2g40a0_tsmc=-199,6126,-732" 										"\x00"
	"pa5ga0_tsmc=-193,6098,-740,-192,6095,-738,-199,5996,-730,-182,6148,-741" 	"\x00"
	"pa5g40a0_tsmc=-193,6098,-740,-192,6095,-738,-199,5996,-730,-182,6148,-741" "\x00"
	"pa5g80a0_tsmc=-193,6098,-740,-192,6095,-738,-199,5996,-730,-182,6148,-741" "\x00"
	"fdss_level_5g=4" 													"\x00"
	"\x00\x00"
};

char *wifi_nvram_ptr = (char *)wifi_nvram_image;
int wifi_nvram_size = (int)sizeof(wifi_nvram_image);