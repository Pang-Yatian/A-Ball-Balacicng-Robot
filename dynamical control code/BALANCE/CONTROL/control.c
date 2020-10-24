#include "control.h"	
#include "filter.h"	
  /**************************************************************************
���ߣ�Pang Yatian
*
**************************************************************************/

u8 Flag_Target,Flag_Target2,Flag_Change;                             //��ر�־λ
u8 temp1;                                               //��ʱ����
float Voltage_Count,Voltage_All; 											 //��ѹ������ر���
int Balance_Pwm_X,Velocity_Pwm_X,Balance_Pwm_Y,Velocity_Pwm_Y,Balance_Pwm_Z;
#define X_PARAMETER          (0.5f)               
#define Y_PARAMETER           (sqrt(3)/2.f)      
#define L_PARAMETER            (1.0f)    
/**************************************************************************
�������ܣ�С���˶���ѧģ��
��ڲ�����X Y Z �����ٶȻ���λ��
����  ֵ����
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
	      Target_A   = -Vx + L_PARAMETER*Vz;
        Target_B   = X_PARAMETER*Vx - Y_PARAMETER*Vy + L_PARAMETER*Vz;
	      Target_C   = X_PARAMETER*Vx + Y_PARAMETER*Vy + L_PARAMETER*Vz;
}
/**************************************************************************
�������ܣ�С���˶� ���˶�ѧ���� ע��ʵ��ע�͵�����������˶�ѧ������ʵ��ʹ�÷Ŵ���3�����Լ�����ûӰ�죬��Ҫ�Ǳ�����ȥ���
��ڲ�����A B C����������ٶ�
����  ֵ����
**************************************************************************/
void Encoder_Analysis(float Va,float Vb,float Vc)
{
		Encoder_X=Va*2-Vb-Vc;
		Encoder_Y=(Vc-Vb)*sqrt(3);
		Encoder_Z=Va+Vb+Vc;
}
/**************************************************************************
�������ܣ����еĿ��ƴ��붼��������
         5ms��ʱ�ж���MPU6050��INT���Ŵ���
         �ϸ�֤���������ݴ����ʱ��ͬ��				 
**************************************************************************/
int EXTI15_10_IRQHandler(void) 
{    
	 if(INT==0)		
	{     
		  EXTI->PR=1<<15;                                                      //���LINE5�ϵ��жϱ�־λ  	        		
		  if(delay_flag==1)
			 {
				 if(++delay_50==10)	 delay_50=0,delay_flag=0;                     //���������ṩ50ms�ľ�׼��ʱ
			 }             
       Flag_Target2=!Flag_Target2;  //�ǶȲɼ���Ƶ��־λ
			 if(Flag_Target2==1)
			 {
			 	Read_DMP();  //�ɼ��Ƕ�
				Key();//ɨ�谴���仯	
		    if(Flag_Show==0)Led_Flash(100);  //===LED��˸;����ģʽ 1s�ı�һ��ָʾ�Ƶ�״̬	
				else Led_Flash(0); //��λ��ģʽ����
				Voltage_All+=Get_battery_volt();   //��β����ۻ��ɼ���ص�ѹ
			  if(++Voltage_Count==100) Voltage=Voltage_All/100,Voltage_All=0,Voltage_Count=0;//��ƽ��ֵ ��ȡ��ص�ѹ	 
         return 0;				 
			 }
			Encoder_A=-Read_Encoder(2);  //===��ȡ��������ֵ Ϊ�˱�֤M�����ٵ�ʱ���׼�����ȶ�ȡ����������
			Encoder_B=-Read_Encoder(3);  //===��ȡ��������ֵ  
			Encoder_C=-Read_Encoder(4);  //===��ȡ��������ֵ+=Encoder_A/20;
				
			Angle_Bias_X =Angle_Balance_X-Angle_Zero_X;		//��ȡY�����ƫ��
		  Angle_Bias_Y =Angle_Balance_Y-Angle_Zero_Y;		//��ȡY�����ƫ��
			Encoder_Analysis(Encoder_A,Encoder_B,Encoder_C);//�Ա����������ݽ������˶�ѧ����
			  Position_X+=Encoder_X;
			  Position_Y+=Encoder_Y;
	  	Read_DMP();   //��ȡ�Ƕ� 
			Balance_Pwm_X= balance_X(Angle_Bias_X,Gyro_Balance_X);//X�������ǿ���
			Balance_Pwm_Y= balance_Y(Angle_Bias_Y,Gyro_Balance_Y);	//Y�������ǿ���
			Balance_Pwm_Z= -balance_Z(Encoder_Z,Gyro_Balance_Z);		//Z������ǿ���
       if(++Flag_Target==4)			//�ٶȿ��Ʒ�Ƶ��־λ 
			  {
				Velocity_Pwm_X=velocity_X(Encoder_X);  //X������ٶȿ���  
				Velocity_Pwm_Y=velocity_Y(Encoder_Y);  //Y������ٶȿ���
					Flag_Target=0;				 
			  }
			  Move_X =Balance_Pwm_X+Velocity_Pwm_X;   //===X����������ۼ�					
			  Move_Y =Balance_Pwm_Y+Velocity_Pwm_Y;   //===Y����������ۼ�					
			  Move_Z=Balance_Pwm_Z;				 //===Z����������ۼ�	
 				Kinematic_Analysis(Move_X,Move_Y,Move_Z);//���˶�ѧ�������õ�A B C���������
			  Xianfu_Pwm(3000);//===PWMƵ���޷�
				Motor_A=Target_A;//ֱ�ӵ���PWMռ�ձ� 
				Motor_B=Target_B;//ֱ�ӵ���PWMռ�ձ�
				Motor_C=Target_C;//ֱ�ӵ���PWMռ�ձ�
		    if(Turn_Off(Voltage)==0)  //===�����ص�ѹ�������쳣
				Set_Pwm(Motor_A,Motor_B,Motor_C);    //��ֵ��PWM�Ĵ���	
					 if(Flag_Zero)  // //������Ϊ�������ֵ
				 {
						 Angle_Zero_Y=Angle_Balance_Y;//Y����Ƕȸ���
						 Angle_Zero_X=Angle_Balance_X;//X����Ƕȸ���
						 Flag_Zero=0;//������̽�ִ��һ�� �ȴ���һ��ָ��
				 }		 				 
 }
	 return 0;			
} 
/**************************************************************************
�������ܣ�Z�����
��ڲ�����Z���ٶȡ�Z����ٶ�
����  ֵ��Z����ƿ���PWM
��    �ߣ�Pang Yatian
**************************************************************************/
int balance_Z(float Velocity,float Gyro)
{  
   float Bias,Target;   //ƫ���Ŀ��ֵ
	 static float Target_Velocity=600;//ң�ص��ٶ�
	 int balance,turn_kd;
	 if(Turn_Left==1)Target=Target_Velocity/sudu,turn_kd=0;   //������ת
	 else if(Turn_Right==1)Target=-Target_Velocity/sudu,turn_kd=0;
	 else Target=0,turn_kd=Turn_Kd;
	 Bias=Velocity-Target;        //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=Turn_Kp*Bias/10+Gyro*turn_kd/100;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ��  
	 return balance;
}
/**************************************************************************
�������ܣ�ֱ��PD����Y
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�Pang Yatian
**************************************************************************/
int balance_Y(float Angle,float Gyro)
{  
   float Bias;
	 int balance;
	 Bias=Angle;        //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=Balance_Kp1*Bias+Gyro*Balance_Kd1/100;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}
/**************************************************************************
�������ܣ�ֱ��PD����
��ڲ������Ƕȡ����ٶ�
����  ֵ��ֱ������PWM
��    �ߣ�Pang Yatian
**************************************************************************/
int balance_X(float Angle,float Gyro)
{  
   float Bias;
	 int balance;
	 Bias=Angle;        //===���ƽ��ĽǶ���ֵ �ͻ�е���
	 balance=Balance_Kp*Bias+Gyro*Balance_Kd/100;   //===����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	 return balance;
}

/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ�����Target_Velocity
��ڲ����������ٶȡ������ٶ�
����  ֵ���ٶȿ���PWM
��    �ߣ�Pang Yatian
**************************************************************************/
int velocity_X(int velocity)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Target_Velocity=1200;//ң�ص��ٶ�
	  static float Encoder_Integral;  //���ֱ���
			if(1==Flag_Left)    	Movement=Target_Velocity/sudu;	           //===ǰ����־λ��1 
		else if(1==Flag_Right)	Movement=-Target_Velocity/sudu;           //===���˱�־λ��1
  	else  Movement=0;
//   //=============�ٶ�PI������=======================//	
		Encoder_Least=velocity;          //�ٶȲɼ�  
		Encoder *= 0.7;		                                                //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.3;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� 
		Encoder_Integral +=Movement;                                      //===����ң�������ݣ�����ǰ������
		if(Encoder_Integral>550000)  	Encoder_Integral=550000;             //===�����޷�
		if(Encoder_Integral<-550000)	Encoder_Integral=-550000;              //===�����޷�	
	  Velocity=Encoder*Velocity_Kp/100+Encoder_Integral*Velocity_Ki/5000;        //===�ٶȿ���	
  	if(Flag_Stop)   Encoder_Integral=0,Encoder=0;      //===����رպ��������
	  return Velocity;
}
/**************************************************************************
�������ܣ��ٶ�PI���� �޸�ǰ�������ٶȣ�����Target_Velocity
��ڲ����������ٶȡ������ٶ�
����  ֵ���ٶȿ���PWM
��    �ߣ�Pang Yatian
**************************************************************************/
int velocity_Y(int velocity)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
	  static float Target_Velocity=1200;
	  static float Encoder_Integral;  
		if(1==Flag_Qian)    	Movement=-Target_Velocity/sudu;	         //===ǰ����־λ��1 
		else if(1==Flag_Hou)	Movement=Target_Velocity/sudu;           //===���˱�־λ��1
  	else  Movement=0;
//   //=============�ٶ�PI������=======================//	
		Encoder_Least=velocity;          //�ٶȲɼ� 
		Encoder *= 0.7;		                                                //===һ�׵�ͨ�˲���       
		Encoder += Encoder_Least*0.3;	                                    //===һ�׵�ͨ�˲���    
		Encoder_Integral +=Encoder;                                       //===���ֳ�λ�� 
		Encoder_Integral +=Movement;                                      //===����ң�������ݣ������˶�
		if(Encoder_Integral>550000)  	Encoder_Integral=550000;            //===�����޷�
		if(Encoder_Integral<-550000)	Encoder_Integral=-550000;           //===�����޷�	
	  Velocity=Encoder*Velocity_Kp1/100+Encoder_Integral*Velocity_Ki1/5000;  //===�ٶȿ���	
  	if(Flag_Stop)   Encoder_Integral=0,Encoder=0;      //===����رպ��������
	  return Velocity;
}


/**************************************************************************
�������ܣ���ֵ��PWM�Ĵ���
��ڲ�����PWM
����  ֵ����
**************************************************************************/
void Set_Pwm(int motor_a,int motor_b,int motor_c)
{
	    int siqua=350, siqub=350, siquc=350;
    	if(motor_a>0)			PWMA=motor_a+siqua, INA=0;
			else  	            PWMA=7200+motor_a-siqua,INA=1;
		
		  if(motor_b>0)			PWMB=7200-motor_b-siqub,INB=1;
			else 	            PWMB=-motor_b+siqub,INB=0;
	
	    if(motor_c>0)			PWMC=motor_c+siquc,INC=0;
			else 	            PWMC=7200+motor_c-siquc,INC=1;
}

/**************************************************************************
�������ܣ�����PWM��ֵ 
��ڲ�������ֵ
����  ֵ����
**************************************************************************/
void Xianfu_Pwm(int amplitude)
{	
    if(Target_A<-amplitude) Target_A=-amplitude;	//�޷�
		if(Target_A>amplitude)  Target_A=amplitude;	
	  if(Target_B<-amplitude) Target_B=-amplitude;	
		if(Target_B>amplitude)  Target_B=amplitude;		
	  if(Target_C<-amplitude) Target_C=-amplitude;	
		if(Target_C>amplitude)  Target_C=amplitude;		
}
/**************************************************************************
�������ܣ�λ��PID���ƹ������ٶȵ�����
��ڲ������ޡ���ֵ
����  ֵ����
**************************************************************************/
void Xianfu_Velocity(int amplitude_A,int amplitude_B,int amplitude_C,int amplitude_D)
{	
    if(Motor_A<-amplitude_A) Motor_A=-amplitude_A;	//λ�ÿ���ģʽ�У�A����������ٶ�
		if(Motor_A>amplitude_A)  Motor_A=amplitude_A;	  //λ�ÿ���ģʽ�У�A����������ٶ�
	  if(Motor_B<-amplitude_B) Motor_B=-amplitude_B;	//λ�ÿ���ģʽ�У�B����������ٶ�
		if(Motor_B>amplitude_B)  Motor_B=amplitude_B;		//λ�ÿ���ģʽ�У�B����������ٶ�
	  if(Motor_C<-amplitude_C) Motor_C=-amplitude_C;	//λ�ÿ���ģʽ�У�C����������ٶ�
		if(Motor_C>amplitude_C)  Motor_C=amplitude_C;		//λ�ÿ���ģʽ�У�C����������ٶ�
}
/**************************************************************************
�������ܣ������޸�С������״̬ 
��ڲ�������
����  ֵ����
**************************************************************************/
void Key(void)
{	
	u8 tmp,tmp2;
	tmp=click_N_Double(75);    //��ȡ������Ϣ
  tmp2=Long_Press();
	if(tmp==1)Flag_Stop=!Flag_Stop;//��������/�رյ��   
	if(tmp==2)Flag_Zero=!Flag_Zero;//˫����ȡ��� 

	if(tmp2==1)Flag_Show=!Flag_Show;//�����л���ʾģʽ 	               
}

/**************************************************************************
�������ܣ��쳣�رյ��
��ڲ�������ѹ
����  ֵ��1���쳣  0������
**************************************************************************/
u8 Turn_Off( int voltage)
{
	    u8 temp;
			if(voltage<2220||Flag_Stop||Angle_Balance_X<-40||Angle_Balance_X>40||Angle_Balance_Y<-40||Angle_Balance_Y>40)//��ص�ѹ���͹رյ��
			{	                                                
      temp=1;      
      PWMA=0;
      PWMB=0;
      PWMC=0;
      INA=0;
      INB=0;
      INC=0;			
      Flag_Stop=1;				
      }
			else
      temp=0;
      return temp;			
}

/**************************************************************************
�������ܣ�����ֵ����
��ڲ�����long int
����  ֵ��unsigned int
**************************************************************************/
u32 myabs(long int a)
{ 		   
	  u32 temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

