#include <EasyLed.h> //Author: Leonel Lopes Parente

//Dat ten chan ket noi cot den #1
  //cau hinh trang thai ban dau la LOW, khi kich hoat la HIGH
EasyLed g1(13, EasyLed::ActiveLevel::High); //green_1
EasyLed y1(12, EasyLed::ActiveLevel::High); //yellow_1
EasyLed r1(11, EasyLed::ActiveLevel::High); //red_1
EasyLed wg1(9, EasyLed::ActiveLevel::High); //walk_green_1
EasyLed wr1(10, EasyLed::ActiveLevel::High); //walk_red_1
//Dat ten chan ket noi cot den #2
EasyLed g2(6, EasyLed::ActiveLevel::High); //green_2
EasyLed y2(5, EasyLed::ActiveLevel::High); //yellow_2
EasyLed r2(4, EasyLed::ActiveLevel::High); //red_2
EasyLed wg2(7, EasyLed::ActiveLevel::High); //walk_green_2
EasyLed wr2(8, EasyLed::ActiveLevel::High); //walk_red_2

//bien chua gia tri cho Timer/Counter 1 - CTC mode
volatile uint8_t state = 0;
volatile uint8_t seconds = 0;

void setup() 
{
  Serial.begin(9600);
  //Bat dau dem 15s xanh #1, do #2
  g1_r2();

  // Cấu hình Timer1 ở CTC mode
  noInterrupts();

  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << WGM12);               // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // Prescaler = 1024
  OCR1A = 15624;                        // 1 giây (với 16 MHz & 1024 prescaler)

  TCNT1 = 0;
  TIMSK1 |= (1 << OCIE1A); // Bật ngắt Compare Match A

  interrupts();
}

void loop() 
{
  // Không xử lý gì ở đây
  if (state == 0)
  {
    wg1.off();
    wg2.on();
  }
    else if (state == 1)
    {
      wg2.flash();
      wg1.off();
    }
    else if (state == 2)
    {
      wg1.on();
      wg2.off();
    }
    else if (state == 3)
    {
      wg1.flash();
    }
}

// Ngắt mỗi 1 giây (khi TCNT1 == OCR1A)
ISR(TIMER1_COMPA_vect) {
  seconds++;
  switch (state) {
    case 0: // LED1 đang sáng trong 15 giây
      if (seconds >= 15) //dem xong 15 giay
      {
        //Dem 3s den vang #1, den do #2 (dem tiep 3 giay con lai)
        y1_r2();
        seconds = 0;
        state = 1;
      }
      break;

    case 1: // LED2 đang sáng trong 3 giây
      if (seconds >= 3) //dem xong 3 giay
      {
        //Bat den do #1, den xanh #2 15 giay (den do #1 se con 3s)
        r1_g2();
        seconds = 0;
        state = 2;
      }
      break;

    case 2: // LED3 đang sáng trong 15 giây
      if (seconds >= 15) //Dem xong 15 giay
      {
        //Dem 3 giay den do #1 (dem tiep 3 giay con lai), den vang #2
        r1_y2();
        seconds = 0;
        state = 3;
      }
      break;

    case 3: // LED4 đang sáng trong 3 giây
      if (seconds >= 3) //Dem xong 3 giay
      {
        //Quay lai dem 15s xanh #1, đo #2
        g1_r2();
        seconds = 0;
        state = 0;
      }
      break;
  }
}

void g1_r2()
{
  //Cot den 1
  g1.on();
  y1.off();
  r1.off();
 
  wr1.on();
  //Cot den 2
  g2.off();
  y2.off();
  r2.on();
  
  wr2.off();
}

void y1_r2()
{
  //Cot den 1
  g1.off();
  y1.on();
  r1.off();
  wg1.off();
  wr1.on();
  //Cot den 2
  g2.off();
  y2.off();
  r2.on();
  
  wr2.off();
}

void r1_g2()
{
  //Cot den 1
  g1.off();
  y1.off();
  r1.on();
  wg1.on();
  wr1.off();
  //Cot den 2
  g2.on();
  y2.off();
  r2.off();
  wg2.off();
  wr2.on();
}

void r1_y2()
{
  //Cot den 1
  g1.off();
  y1.off();
  r1.on();
  wr1.off();
  //Cot den 2
  g2.off();
  y2.on();
  r2.off();
  wg2.off();
  wr2.on();
}