#include "sys.h"
  /**************************************************************************
���ߣ�Pang Yatian
*
**************************************************************************/ 
u8 Flag_Stop=1,Flag_Show,Flag_Zero;      //ֹͣ��־λ�� ��ʾ��־λ Ĭ��ֹͣ ��ʾ��
int Encoder_A,Encoder_B,Encoder_C;          //���������������
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Turn_Left,Turn_Right;                  
long int Motor_A,Motor_B,Motor_C;        //���PWM����
long int Target_A,Target_B,Target_C;     //���Ŀ��ֵ
float Encoder_X,Encoder_Y,Encoder_Z,Position_X,Position_Y;;
int Voltage;                             //��ص�ѹ������صı���                       
u8 delay_50,delay_flag,sudu=1;                          //��ʱ��ر���
u8 Turn_Flag,PID_Send;  //CAN�ʹ��ڿ�����ر���
float Angle_Balance_X,Angle_Balance_Y,Angle_Balance_Z,Gyro_Balance_X,Gyro_Balance_Z,Gyro_Balance_Y,Move_X,Move_Y,Move_Z;   //����ǶȺ�XYZ��Ŀ���ٶ�
float	Balance_Kp=255,Balance_Kd=120,Balance_Kp1=265,Balance_Kd1=120,Velocity_Kp=78,Velocity_Ki=50,Velocity_Kp1=123,Velocity_Ki1=60,Turn_Kp=25,Turn_Kd=87;  //����PID���� 151 88
int Angle_Zero_X, Angle_Zero_Y,Angle_Bias_X, Angle_Bias_Y;
int main(void)
{ 
	Stm32_Clock_Init(9);            //=====ϵͳʱ������
	delay_init(72);                 //=====��ʱ��ʼ��
	JTAG_Set(JTAG_SWD_DISABLE);     //=====�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);           //=====��SWD�ӿ� �������������SWD�ӿڵ���
	LED_Init();                     //=====��ʼ���� LED ���ӵ�Ӳ���ӿ�
	KEY_Init();                     //=====������ʼ��
	OLED_Init();                    //=====OLED��ʼ��
	uart_init(72,128000);           //=====����1��ʼ��
	uart2_init(36,9600);            //=====����2��ʼ��
 // uart3_init(36,115200);          //=====����3��ʼ�� 
	Encoder_Init_TIM2();            //=====�������ӿ�
	Encoder_Init_TIM3();            //=====�������ӿ�
	Encoder_Init_TIM4();            //=====��ʼ��������C
	Encoder_Init_TIM5();            //=====��ʼ��������D
	Adc_Init();                     //=====adc��ʼ��
	IIC_Init();                     //=====IIC��ʼ��
  MPU6050_initialize();           //=====MPU6050��ʼ��	
  DMP_Init();                     //=====��ʼ��DMP     
	MiniBalance_PWM_Init(7199,0);   //=====��ʼ��PWM 10KHZ�������������
  EXTI_Init();                    //=====MPU6050 5ms��ʱ�жϳ�ʼ��
	while(1)
		{		
	    if(Flag_Show==0)           //ʹ��MiniBalance APP��OLED��ʾ��
			{
				APP_Show();	              
				oled_show();             //===��ʾ����
	    }
    	else                       //ʹ��MiniBalance��λ�� ��λ��ʹ�õ�ʱ����Ҫ�ϸ��ʱ�򣬹ʴ�ʱ�ر�app��ز��ֺ�OLED��ʾ��
  		{
				DataScope();              //����MiniBalance��λ��
			}
			delay_flag=1;	
			delay_50=0;
			while(delay_flag);	       //ͨ��MPU6050��INT�ж�ʵ�ֵ�50ms��׼��ʱ				
		} 
}
