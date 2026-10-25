#include <reg51.h>
#include <intrins.h>

#define LeftShift(x) _crol_(x, 1)
#define LCD_Data P2
sbit LCD_RS = P1 ^ 0;
sbit LCD_RW = P1 ^ 1;
sbit LCD_E = P1 ^ 2;

void delay(unsigned int t);
void delay_us(unsigned int t);
void write_data_LCD(char WDLCD);
void write_command_LCD(char WCLCD);
void LCD_init(void);
unsigned char getKB(unsigned char var);

unsigned char que[4]=' ';
unsigned char ans[4]=' ';
unsigned char recei[4]=' ';
char flag = 0;  
char index2 = 0; 
char ques=0, ques_r=0;
unsigned char a,b,c,x,y,z,start=0,c1=0,c2=0;
char m0=0, s1=0;
char i, value;

// 通訊狀態旗標
char ack_flag = 0;
char nack_flag = 0;

void main(void)
{
    unsigned char kb_buf, column, tmp;
    
    LCD_init();

    IE = 0x90;
    SCON = 0x50;
    TMOD = 0x20;
    TH1 = 253;
    TR1 = 1;

    while (1)
    {
        kb_buf = 0xFE;
        for (column = 0; column < 4; column++)
        {
            P0 = kb_buf;
            tmp = P0;
            if ((tmp & 0xF0) != 0xF0)
            {
                delay(1);
                tmp = P0;
                if ((tmp & 0xF0) != 0xF0)
                {
                    if(getKB(tmp)!='D' && getKB(tmp)!='E' && getKB(tmp)!='F' && ques<4 && start==0 && getKB(tmp)>='0')
                    {
                        if(getKB(tmp)!= que[3] && getKB(tmp)!= que[2] && getKB(tmp)!= que[1] && getKB(tmp)!= que[0])
                        {
                            que[0]= que[1]; que[1]= que[2]; que[2]= que[3]; que[3] = getKB(tmp);
                            delay(50);
                            write_data_LCD(que[3]);
                            ques++;
                        }
                    }
                    if(getKB(tmp)!='D' && getKB(tmp)!='E' && getKB(tmp)!='F' && ques_r<4 && start==1 && getKB(tmp)>='0' && s1==1)
                    {									
                        if(getKB(tmp)!= recei[3] && getKB(tmp)!= recei[2] && getKB(tmp)!= recei[1] && getKB(tmp)!= recei[0])
                        {		
                            recei[0] = recei[1]; recei[1] = recei[2]; recei[2] = recei[3]; recei[3] = getKB(tmp);
                            delay(50);
                            write_data_LCD(recei[3]);
                            ques_r++;
                        }
                    }
                    if(getKB(tmp)=='D' && start==0 && s1==0)
                    {
                        if(ques==4)
                        {
                            SBUF='s';
                            start=1;
                        }
                        delay(100);
                    }
                    if(getKB(tmp)=='E')
                    {
                        if(start==0)
                        {
                            write_command_LCD(0x01);
                            que[3]=que[2]; que[2]=que[1]; que[1]=que[0]; que[0]=0x00;
                            ques--;
                            if(ques<0) 
															ques=0;
                            write_command_LCD(0x80);
														
                            for(y=0;y<ques;y++)
														{ 
															write_data_LCD(que[4-ques+y]); 
															delay(10); 
														}
                        }
                        if(start==1 && s1==1)
                        {
                            for(c=0;c<16;c++) 
														{ 
															write_command_LCD(0x80+c); 
															write_data_LCD(' '); 
															delay(2); 
														}
                            recei[3]=recei[2]; recei[2]=recei[1]; recei[1]=recei[0]; recei[0]=0x00;
                            ques_r--;
														
                            if(ques_r<0) 
															ques_r=0;
														
                            write_command_LCD(0x80);
														
                            for(z=0;z<ques_r;z++) 
															write_data_LCD(recei[4-ques_r+z]);
                        }
                    }
                    
                    // 發送端加入 CheckSum 與 Timeout 重傳 
                    if(getKB(tmp)=='F' && s1==1 && ques_r==4)
                    {
                        char retry_count = 3; // 最多重傳 3 次
                        unsigned char checksum;
                        unsigned int timeout_counter;
                        
                        while(retry_count > 0)
                        {
                            checksum = 0;
                            ack_flag = 0;
                            nack_flag = 0;
                            
                            // 發送 4 Bytes 資料並計算 XOR
                            for(x=0; x<4; x++)
                            {		
                                SBUF = recei[x];
                                checksum ^= recei[x];
                                delay(100);
                            }
                            
                            // 發送第 5 Byte (CheckSum)
                            SBUF = checksum;
                            delay(100);
                            
                            // Timeout 等待機制 (等待接收端回傳 ACK/NACK)
                            timeout_counter = 30000; 
                            while(timeout_counter > 0)
                            {
                                if(ack_flag == 1 || nack_flag == 1) break;
                                timeout_counter--;
                            }
                            
                            // 判斷傳輸結果
                            if(ack_flag == 1) {
                                break; // 成功收到 ACK，跳出重傳迴圈
                            }
                            
                            // 若為 nack_flag==1 或超時 (timeout_counter==0)，迴圈會繼續執行重傳
                            retry_count--;
                        }
                    }
                    // =================================================================================
                }
                do { tmp = P0; } while ( (tmp & 0xF0) != 0xF0 );
            } 
            kb_buf = LeftShift(kb_buf);
        } 
				
        if(flag==1)
        {
            for(x=0;x<4;x++)
            {
                for(y=0;y<4;y++)
                {
                    if((que[x]==ans[y])&& x==y) c1++;
                    else if(que[x]==ans[y]) c2++;
                }
            }
            flag=0;
            SBUF=10*c1+c2;
            c1=0; c2=0;
        }
    } 
}

void uart_isr(void) interrupt 4
{
    if (TI == 1) 
    {
        TI = 0; 
        if(SBUF=='s') 
					m0=1;
    }
    if (RI == 1) 
    {
        RI = 0;
        if(SBUF=='s')
        { 
						s1=1; 
						start=1; 
						ques_r=ques;
						for(x=0;x<4;x++) 
						{ 
							recei[x]=que[x]; 
							delay(5); 
						}
        }
        // 接收端加入 ACK/NACK 與 驗證邏輯 
        else if(SBUF == 'O') // 收到 ACK (字元 'O')
        {
           ack_flag = 1;
        }
        else if(SBUF == 'E') // 收到 NACK (字元 'E')
        {
           nack_flag = 1;
        }
        else if((SBUF>=0x30 && SBUF<=0x39) || index2 == 4)
        {
            if(index2 < 4) // 接收前 4 個數字
            {
							ans[index2] = SBUF;
							index2++;
            }
            else if(index2 == 4) // 接收第 5 個 Byte (CheckSum) 並進行比對
            {
							unsigned char calc_check = ans[0] ^ ans[1] ^ ans[2] ^ ans[3];
							
							if(SBUF == calc_check) // 資料正確無誤
							{
									SBUF = 'O'; // 回傳 ACK
									delay(5);
									for(a=0;a<4;a++)
									{
											write_command_LCD(0xc0+a);
											write_data_LCD(ans[a]);
											delay(1);
									}
									flag = 1;
							}
							else // 資料有誤或漏字元
							{
									SBUF = 'E'; // 回傳 NACK，要求重傳
									delay(5);
							}
							index2 = 0; // 無論成功失敗，強制作為狀態重置，防止錯位
            }
        }
        
        else if(SBUF>=0)
        {
            c1=SBUF/10; c2=SBUF%10;
            delay(5);
            write_command_LCD(0xc0);
            write_data_LCD(c1+0x30); write_data_LCD('A');
            write_data_LCD(c2+0x30); write_data_LCD('B');
            delay(20);
        }
    }
}

unsigned char getKB(unsigned char var)
{
    switch (var)
    {
    // column1 P0.0
    case 0xDE: return '8';    // 1101 1110 => P0.5 P0.0 => 8
    case 0xBE: return '4';    // 1011 1110 => P0.6 P0.0 => 4
    case 0x7E: return '0';    // 0111 1110 => P0.7 P0.0 => 0

    // column2 P0.1
    case 0xED: return 'D';   // 1110 1101 => P0.4 P0.1 => D
    case 0xDD: return '9';    // 1101 1101 => P0.5 P0.1 => 9
    case 0xBD: return '5';    // 1011 1101 => P0.6 P0.1 => 5
    case 0x7D: return '1';    // 0111 1101 => P0.7 P0.1 => 1

    // column3 P0.2
    case 0xEB: return 'E';   // 1110 1011 => P0.4 P0.2 => E
    case 0xBB: return '6';    // 1011 1011 => P0.6 P0.2 => 6
    case 0x7B: return '2';    // 0111 1011 => P0.7 P0.2 => 2

    // column4 P0.3
    case 0xE7: return 'F';   // 1110 0111 => P0.4 P0.3 => F
    case 0xB7: return '7';    // 1011 0111 => P0.6 P0.3 => 7
    case 0x77: return '3';    // 0111 0111 => P0.7 P0.3 => 3

    default: return ' ';
    }
}

void delay(unsigned int t)
{
    unsigned int i;
    while (t--)
    {
        i = 255;
        while (i--);
    }
}

void delay_us(unsigned int t)
{
    while (t--);
}

//寫資料到LCD副程式
void write_data_LCD(char WDLCD)
{
    LCD_Data = WDLCD;
    LCD_RS = 1;  // RS=1 資料暫存器
    LCD_RW = 0;  // RW=0 寫入
    LCD_E = 1;
    delay_us(100);  //延遲等訊號穩定
    LCD_E = 0;
    delay_us(100);  //延遲等訊號處裡完畢
}

//寫指令到LCD副程式
void write_command_LCD(char WCLCD)
{
    LCD_Data = WCLCD;
    LCD_RS = 0;
    LCD_RW = 0;
    LCD_E = 1;
    delay_us(100);  //延遲等訊號穩定
    LCD_E = 0;
    delay_us(100);  //延遲等訊號處裡完畢
}

// LCD初始化
void LCD_init(void)
{
    write_command_LCD(0x38);  //設定LCD介面8位元，2行，5*7字型
    write_command_LCD(0x06);  //設定寫資料至LCD時，顯示不要動，游標向右移動
    write_command_LCD(0x0F);  //設定螢幕顯示及游標閃爍

    write_command_LCD(0x01);  //清除顯示器
    delay_us(5000);
}