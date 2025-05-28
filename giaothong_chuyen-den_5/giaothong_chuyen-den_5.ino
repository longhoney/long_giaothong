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

//Bien chua gia tri cho Timer/Counter 1 - normal mode
volatile uint8_t state = 0;
volatile uint8_t overflowCount = 0;

const uint8_t FULL_OVERFLOWS_15S = 3;
const uint16_t TCNT1_FOR_15S_LAST = 27751; // Cho ~2.418s còn lại (3 tràn đầy + 1 rút ngắn)
const uint16_t TCNT1_FOR_3S = 18661;       // 3 giây

void setup() 
{
  Serial.begin(9600);
  //Start count 15s green_1, red_2
  g1_r2();
  //Khoi setup Timer/Counter 1 - normal mode
  noInterrupts(); // Tạm thời tắt ngắt khi cấu hình
  // Cấu hình Timer1 - Normal Mode, prescaler 1024
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler = 1024

  TCNT1 = 0; // Bắt đầu từ 0
  TIMSK1 |= (1 << TOIE1); // Bật ngắt tràn

  interrupts();
}

void loop() 
{
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

ISR(TIMER1_OVF_vect) {
  switch (state) {
    case 0: //counting 15s green_1, red_2
      overflowCount++;
      if (overflowCount == FULL_OVERFLOWS_15S) {
        TCNT1 = TCNT1_FOR_15S_LAST; // Lần rút ngắn cuối
      } else if (overflowCount > FULL_OVERFLOWS_15S) //dem xong 15s
      {
        //3s den vang #1, den do #2 (dem tiep 3 giay con lai)
        y1_r2();
        TCNT1 = TCNT1_FOR_3S; //Bat dau dem
        overflowCount = 0;
        state = 1;
      }
      break;

    case 1: //Dem xong 3 giay den vang #1, den do #2
      //Bat den do #1, den xanh #2 15 giay (den do #1 se con 3s)
      r1_g2();
      TCNT1 = 0;
      overflowCount = 0;
      state = 2; //Bat dau dem 15s tiep theo
      break;

    case 2: //Dem 15 giay den do #1, den xanh #2 15 giay (den do #1 se con 3s)
      overflowCount++;
      if (overflowCount == FULL_OVERFLOWS_15S) {
        TCNT1 = TCNT1_FOR_15S_LAST;
      } else if (overflowCount > FULL_OVERFLOWS_15S) //dem xong 15s
      {
        // Bat dau dem 3 giay den do #1 (dem tiep 3 giay con lai), den vang #2
        r1_y2();
        TCNT1 = TCNT1_FOR_3S; 
        overflowCount = 0;
        state = 3;
      }
      break;

    case 3: //Dem xong 3 giay den do #1, den vang #2
      //Quay lai dem 15s xanh #1, đo #2
      g1_r2();
      TCNT1 = 0;
      overflowCount = 0;
      state = 0;
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