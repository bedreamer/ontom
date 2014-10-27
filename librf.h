#ifndef __RDRF_H
#define __RDRF_H

#define uc unsigned char
#ifndef false
#define false 0
#define true  1
#endif

int  dc_readdevsnr(int idComDev,unsigned char *snr);
int dc_readreadersnr(int idComDev,unsigned char *snr);
int dc_getver(int icdev,unsigned char *sver);
int dc_update(int icdev);
int dc_init(int port,long baud);
int dc_exit(int icdev);
int dc_config(int icdev,unsigned char _Mode,unsigned char _Baud);

int dc_config_card(int icdev,unsigned char cardtype);
int dc_request_b(int icdev,unsigned char _Mode,
							   unsigned char AFI, 
		                       unsigned char Num, 
		                       unsigned char *ATQB);
		                       
int dc_hltb(int icdev,unsigned char *PUPI);
		                       
int dc_attrib(int icdev,unsigned char *PUPI, unsigned char CID);
		                       
		                       
int dc_request(int icdev,unsigned char _Mode,unsigned int *TagType);
int dc_anticoll(int icdev,unsigned char _Bcnt,unsigned long *_Snr);
int dc_select(int icdev,unsigned long _Snr,unsigned char *_Size);
int dc_authentication(int icdev,unsigned char _Mode,unsigned char _SecNr);
int dc_halt(int icdev);
int dc_read(int icdev,unsigned char _Adr,unsigned char *_Data);
int dc_read_hex(int icdev,unsigned char _Adr,char *_Data);
int dc_write(int icdev,unsigned char _Adr,unsigned char *_Data);
int dc_write_hex(int icdev,unsigned char _Adr,char *_Data);
int dc_load_key(int icdev,unsigned char _Mode,unsigned char _SecNr,unsigned char *_NKey);
int dc_load_key_hex(int icdev,unsigned char _Mode,unsigned char _SecNr,char *_NKey);
int dc_increment(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_decrement(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_decrement_ml(int icdev,unsigned int _Value);
int dc_restore(int icdev,unsigned char _Adr);
int dc_transfer(int icdev,unsigned char _Adr);
int dc_card(int icdev,unsigned char _Mode,unsigned long *_Snr);
int dc_initval(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_initval_ml(int icdev,unsigned int _Value);
int dc_readval(int icdev,unsigned char _Adr,unsigned long *_Value);
int dc_readval_ml(int icdev,unsigned int *_Value);
int dc_changeb3(int icdev,unsigned char _SecNr,unsigned char *_KeyA,unsigned char _B0,unsigned char _B1,unsigned char _B2,unsigned char _B3,unsigned char _Bk,unsigned char *_KeyB);
int dc_get_status(int icdev,unsigned char *_Status);
int dc_clr_control_bit(int icdev,unsigned char _b);
int dc_set_control_bit(int icdev,unsigned char _b);
int dc_reset(int icdev,unsigned int _Msec);
int dc_HL_decrement(int icdev,unsigned char _Mode,unsigned char _SecNr,unsigned long _Value,unsigned long _Snr,unsigned long *_NValue,unsigned long *_NSnr);
int dc_HL_increment(int icdev,unsigned char _Mode,unsigned char _SecNr,unsigned long _Value,unsigned long _Snr,unsigned long *_NValue,unsigned long *_NSnr);
int dc_HL_write(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long *_Snr,unsigned char *_Data);
int dc_HL_writehex(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long *_Snr,unsigned char *_Data);
int dc_HL_read(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long _Snr,unsigned char *_Data,unsigned long *_NSnr);
int dc_HL_readhex(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long _Snr,unsigned char *_Data,unsigned long *_NSnr);
int dc_HL_initval(int icdev,unsigned char _Mode,unsigned char _SecNr,unsigned long _Value,unsigned char *_Snr);
int dc_beep(int icdev,unsigned short _Msec);
int dc_disp_str(int icdev,char *dispstr);
int dc_HL_authentication(int icdev,unsigned char reqmode,unsigned long snr,unsigned char authmode,unsigned char secnr);
int dc_srd_eeprom(int icdev,int offset,int lenth,unsigned char *rec_buffer);
int dc_swr_eeprom(int icdev,int offset,int lenth,unsigned char* send_buffer);
int swr_alleeprom(int icdev,int offset,int lenth,unsigned char* snd_buffer);
int dc_srd_eepromhex(int icdev,int offset,int lenth,unsigned char *rec_buffer);
int dc_swr_eepromhex(int icdev,int offset,int lenth,unsigned char* send_buffer);
int srd_alleeprom(int icdev,int offset,int lenth,unsigned char *receive_buffer);
int dc_check_write(int icdev,unsigned long Snr,unsigned char authmode,unsigned char Adr,unsigned char * _data);
int dc_check_writehex(int icdev,unsigned long Snr,unsigned char authmode,unsigned char Adr,unsigned char * _data);
int dc_authentication_2(int icdev,unsigned char _Mode,unsigned char KeyNr,unsigned char Adr);
int dc_gettime(int icdev,unsigned char *time);
int dc_gettimehex(int icdev,char *time);
int dc_settime(int icdev,unsigned char *time);
int dc_settimehex(int icdev,char *time);
int dc_setbright(int icdev,unsigned char bright);
int dc_ctl_mode(int icdev,unsigned char mode);
int dc_disp_mode(int icdev,unsigned char mode);
int dcdeshex(unsigned char *key,unsigned char *sour,unsigned char *dest,int m);
int dc_light(int icdev,unsigned short _OnOff);
int dc_high_disp(int icdev,unsigned char offset,unsigned char displen,unsigned char *dispstr);
int dc_setcpu(int icdev,unsigned char _Byte);
int dc_setcpupara(int icdev,unsigned char cputype,unsigned char cpupro,unsigned char cpuetu);
int dc_cpureset(int icdev,unsigned char *rlen,unsigned char *databuffer);
int dc_cpuapdusource(int icdev,unsigned char slen,unsigned char * sendbuffer,unsigned char *rlen,unsigned char * databuffer);
int dc_cpuapdu(int icdev,unsigned char slen,unsigned char * sendbuffer,unsigned char *rlen,unsigned char * databuffer);
int dc_cpureset_hex(int icdev,unsigned char *rlen, char *databuffer);
int dc_cpuapdusource_hex(int icdev,unsigned char slen, char * sendbuffer,unsigned char *rlen, char * databuffer);
int dc_cpuapdu_hex(int icdev,unsigned char slen, char * sendbuffer,unsigned char *rlen, char * databuffer);
int dc_cpudown(int icdev);
int dc_setbptime(int icdev,unsigned char _Byte);
int dc_card_hex(int icdev,unsigned char _Mode,unsigned char *snrstr);
int dc_set_addr(unsigned char saddr);
int dc_init_485(int port,long baud);
int dc_change_addr(int icdev,unsigned char saddr);
int dc_read_all(int icdev,unsigned char *_Data);
int dc_write_all(int icdev,unsigned char *_Data);
int dc_read_allhex(int icdev, char *_Data);
int dc_write_allhex(int icdev, char *_Data);
int dc_set_autoflag(int icdev,unsigned char _Byte);
int dc_read_one(int icdev,unsigned char _SecNr,unsigned char *_Data);
int dc_write_one(int icdev,unsigned char _SecNr,unsigned char *_Data);
int dc_read_onehex(int icdev,unsigned char _SecNr,char *_Data);
int dc_write_onehex(int icdev,unsigned char _SecNr,char *_Data);
int dc_read_auto(int icdev,unsigned char *_Data);
int dc_read_autohex(int icdev,char *_Data);
int dc_anticoll2(int icdev,unsigned char _Bcnt,unsigned long *_Snr);
int dc_select2(int icdev,unsigned long _Snr,unsigned char *_Size);
int dc_pro_reset(int icdev,unsigned char *rlen,unsigned char *receive_data);
int dc_pro_resethex(int icdev,unsigned char *rlen, char *receive_data);
int dc_pro_command(int idComDev,unsigned int slen,
								 unsigned char * sendbuffer,unsigned int *rlen,
								 unsigned char * databuffer,unsigned char timeout);
int dc_pro_commandsource(int idComDev,unsigned char slen,
									   unsigned char * sendbuffer,unsigned char *rlen,
									   unsigned char * databuffer,unsigned char timeout);
int dc_pro_commandsourcehex(int idComDev,unsigned char slen, 
										  char * sendbuffer,unsigned char *rlen, 
										  char * databuffer,unsigned char timeout);
int dc_pro_commandhex(int idComDev,unsigned int slen,
									char * sendbuffer,unsigned int *rlen,
									char * databuffer,unsigned char timeout);
int dc_pro_halt(int icdev);
int dc_pro_commandlink(int idComDev,unsigned int slen,
								 unsigned char * sendbuffer,unsigned int *rlen,
								 unsigned char * databuffer,unsigned char timeout,
								 unsigned char FG);
int dc_pro_commandlink_hex(int idComDev,unsigned int slen,
										 unsigned char * sendbuffer,unsigned int *rlen,
										 unsigned char * databuffer,unsigned char timeout,
										 unsigned char FG);

int dc_pro_reset_hex(int icdev,unsigned char *rlen, char *receive_data);
int dc_pro_command_hex(int idComDev,unsigned int slen, 
									 char * sendbuffer,unsigned int *rlen, 
									 char * databuffer,unsigned char timeout);
int dc_pro_commandsource_hex(int idComDev,unsigned char slen, 
										   char * sendbuffer,unsigned char *rlen, 
										   char * databuffer,unsigned char timeout);
										   
//接触式卡函数2007/02/27
int hex_a(unsigned char *hex,unsigned char *a,int length);
int a_hex(unsigned char *a,unsigned char *hex,int len);		
//4428								   									 										 
int dc_read_4428(int icdev,int offset,int lenth,unsigned char * receive_buffer);										 								 									
int dc_read_4428_hex(int icdev,int offset,int lenth,unsigned char* receive_buffer);
int dc_write_4428(int icdev,int offset,int lenth,unsigned char * snd_buffer);
int dc_write_4428_hex(int icdev,int offset,int lenth,unsigned char* send_buffer);
int dc_verifypin_4428(int icdev,unsigned char *passwd);
int dc_verifypin_4428_hex(int icdev,unsigned char *passwd);
int dc_readpin_4428(int icdev,unsigned char *passwd);
int dc_readpin_4428_hex(int icdev,unsigned char *passwd);
int dc_readpincount_4428(int icdev);
int dc_changepin_4428(int icdev,unsigned char *passwd);
int dc_changepin_4428_hex(int icdev,unsigned char *passwd);
int dc_Check_4428(int icdev);
//4442
int dc_read_4442(int icdev,int offset,int lenth,unsigned char * receive_buffer);
int dc_read_4442_hex(int icdev,int offset,int lenth,unsigned char* receive_buffer);
int dc_write_4442(int icdev,int offset,int lenth,unsigned char * snd_buffer);
int dc_write_4442_hex(int icdev,int offset,int lenth,unsigned char* send_buffer);
int dc_verifypin_4442(int icdev,unsigned char *passwd);
int dc_verifypin_4442_hex(int icdev,unsigned char *passwd);
int dc_readpin_4442(int icdev,unsigned char *passwd);
int dc_readpin_4442_hex(int icdev,unsigned char *passwd);
int dc_readpincount_4442(int icdev);
int dc_changepin_4442(int icdev,unsigned char *passwd);
int dc_changepin_4442_hex(int icdev,unsigned char *passwd);
int dc_Check_4442(int icdev);
//24C
int dc_read_24c(int icdev,int offset,int lenth,unsigned char * receive_buffer);
int dc_read_24c_hex(int icdev,int offset,int lenth,unsigned char* receive_buffer);
int dc_read_24c64(int icdev,int offset,int lenth,unsigned char * receive_buffer);
int dc_read_24c64_hex(int icdev,int offset,int lenth,unsigned char* receive_buffer);
int dc_write_24c(int icdev,int offset,int lenth,unsigned char * snd_buffer);
int dc_write_24c_hex(int icdev,int offset,int lenth,unsigned char* send_buffer);
int dc_write_24c64(int icdev,int offset,int lenth,unsigned char * snd_buffer);
int dc_write_24c64_hex(int icdev,int offset,int lenth,unsigned char* send_buffer);
int Test_24Card(int icdev);
int dc_Check_24C01(int icdev);
int dc_Check_24C02(int icdev);
int dc_Check_24C04(int icdev);
int dc_Check_24C08(int icdev);
int dc_Check_24C16(int icdev);
int dc_Check_24C64(int icdev);
int dc_Check_CPU(int icdev);
int dc_CheckCard(int icdev);
int dc_card_double(int icdev,unsigned char _Mode,unsigned char *_Snr);
int dc_card_double_hex(int icdev,unsigned char _Mode,unsigned char *_Snr);
int dc_des(unsigned char *key,unsigned char *sour,unsigned char *dest,int m);
int dc_mfdes_auth(int icdev,unsigned char keyno,unsigned char keylen,unsigned char *authkey,
								unsigned char *randAdata,unsigned char *randBdata);
int dc_mfdes_auth_hex(int icdev,unsigned char keyno,unsigned char keylen,unsigned char *authkey,
									unsigned char *randAdata,unsigned char *randBdata);
int dc_switch_win(int icdev);

																		  									   								 
#endif
