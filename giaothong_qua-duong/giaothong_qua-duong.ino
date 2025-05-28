#include <EasyLed.h> //Author: Leonel Lopes Parente

// Nút nhấn = D2 (INT0) - PullUp

//Dat ten chan ket noi cot den #1
  //cau hinh trang thai ban dau la LOW, khi kich hoat la HIGH
EasyLed g1(13, EasyLed::ActiveLevel::High); //green_1
EasyLed y1(12, EasyLed::ActiveLevel::High); //yellow_1
EasyLed r1(11, EasyLed::ActiveLevel::High); //red_1
EasyLed wg1(9, EasyLed::ActiveLevel::High); //walk_green_1
EasyLed wr1(10, EasyLed::ActiveLevel::High); //walk_red_1

//Bien chua gia tri cho Timer/Counter 1 - normal mode
volatile uint8_t state = 0;
volatile uint8_t overflowCount = 0;
volatile bool inSequence = false;

const uint8_t FULL_OVERFLOWS_15S = 3;
const uint8_t FULL_OVERFLOWS_12S = 2;
const uint16_t TCNT1_FOR_15S_LAST = 27751; // Cho ~2.418s còn lại (3 tràn đầy + 1 rút ngắn)
const uint16_t TCNT1_FOR_12S_LAST = 9098; // Cho ~2.418s còn lại (3 tràn đầy + 1 rút ngắn)
const uint16_t TCNT1_FOR_3S = 18661;       // 3 giây

void setup()
{
  
  Serial.begin(9600);
  //Cau hinh nut nhan qua duong
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), startSequence, FALLING); // ngắt ngoài
  attachInterrupt(digitalPinToInterrupt(3), startSequence, FALLING); // ngắt ngoài
  //truong hop bam nut 2 roi, dang chay lenh nut 2, nguoi khac bam nut 3 thi sao?
    //-->Da thu nghiem, khong bi anh huong
    //-->Dang chay chuong trinh, bam nut cung khong anh huong
  //Khoi setup Timer/Counter 1 - normal mode
  noInterrupts(); // Tạm thời tắt ngắt khi cấu hình
  // Cấu hình Timer1 - Normal Mode, prescaler 1024
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 |= (1 << TOIE1); // Bật ngắt tràn
  interrupts();
}

void loop()
{
  if (state == 0)
  {
    //den vang nhap nhay canh bao
    y1.flash();
    wg1.off();
  }
    else if (state == 4)
    {
      wg1.flash();
    }
  Serial.print("state ==");
  Serial.println(state);      
}

void startSequence()
{
  if (inSequence) return; // Đang chạy rồi, bỏ qua
    inSequence = true;
    state = 1;
    overflowCount = 0;
  //Chuyen sang 15s xanh
  y1.off(); //tat den vang
  g1.on();  //bat den xanh
  wr1.on(); //bat den di bo do
  wg1.off();
  TCNT1 = 0;
  TCCR1B |= (1 << CS12) | (1 << CS10); // Prescaler = 1024
}

ISR(TIMER1_OVF_vect) {
  if (!inSequence) return;

  switch (state) {
    case 1: // LED3 sáng 15s
      overflowCount++;
      if (overflowCount == FULL_OVERFLOWS_15S) {
        TCNT1 = TCNT1_FOR_15S_LAST;
      } else if (overflowCount > FULL_OVERFLOWS_15S) 
      {
        //Dem xong 15s xanh
        g1.off();   //tat den xanh
        y1.on(); //bat den vang
        wr1.on(); //bat den di bo do
        wg1.off();
        TCNT1 = TCNT1_FOR_3S;    //đếm 3s
        overflowCount = 0;
        state = 2;
      }
      break;

    case 2: //Dem xong 3s vang
      y1.off();     //Tat den vang
      r1.on();    //Bat den do
      wr1.off(); 
      wg1.on();  //bat den di bo xanh
      TCNT1 = 0; //dem 12s
      overflowCount = 0; 
      state = 3;
      break;

    case 3:   //Dang dem 12s den đo
      overflowCount++;
      if (overflowCount == FULL_OVERFLOWS_12S) {
        TCNT1 = TCNT1_FOR_12S_LAST; //Dem xong 12s den do
      } else if (overflowCount > FULL_OVERFLOWS_12S) //dem xong 12s
      {
        //Dem tiep 3s den do, nhay den di bo xanh
        y1.off();     //Tat den vang
        r1.on();    //Bat den do
        wr1.off(); 
        TCNT1 = TCNT1_FOR_3S; //Bat dau dem
        overflowCount = 0;
        state = 4;
      }
      break;

    case 4: //Dem xong 3s den do con lai
      // Kết thúc chuỗi, quay về ban đầu
        r1.off();
        TCCR1B &= ~((1 << CS12) | (1 << CS11) | (1 << CS10)); // Dừng Timer1
        overflowCount = 0;
        state = 0;
        inSequence = false;
      
      break;
  }
}
