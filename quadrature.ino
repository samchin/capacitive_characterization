#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#define MultiS16X16toH16(intRes, intIn1, intIn2) \
 do { \
  asm volatile ( \
    "clr r26              \n\t" \
    "mul %A1, %A2         \n\t" \
    "mov r27, r1          \n\t" \
    "muls %B1, %B2        \n\t" \
    "movw %A0, r0         \n\t" \
    "mulsu %B2, %A1       \n\t" \
    "sbc %B0, r26         \n\t" \
    "add r27, r0          \n\t" \
    "adc %A0, r1          \n\t" \
    "adc %B0, r26         \n\t" \
    "mulsu %B1, %A2       \n\t" \
    "sbc %B0, r26         \n\t" \
    "add r27, r0          \n\t" \
    "adc %A0, r1          \n\t" \
    "adc %B0, r26         \n\t" \
    "clr r1               \n\t" \
    : "=&r" (intRes) \
    : "a" (intIn1), "a" (intIn2) \
    : "r26", "r27" \
  ); \
} while (0)


// === Global Constants ===
const uint16_t nsamps = 32;  // Number of sample sets

// === Global Variables ===
volatile uint16_t adc_value = 0;           // Latest ADC reading
volatile uint8_t adc_phase = 0;            // Current sampling phase
volatile uint8_t new_sample_flag = 0;      // Flag for new sample ready

// Working variables for accumulation (used outside ISR)
int16_t in_phase = 0;
int16_t quadrature = 0;

int16_t inphase_sq = 0;
int16_t quad_sq = 0;

// ADC Measurements
uint8_t count = 0;
volatile uint8_t count_done = 0;
uint16_t adc_data[64] = {0}; 

const uint8_t double_average = 16;

void setup() {
  Serial.begin(115200);
  delay(100);
  // Serial.println("Arduino Alive");
  pinMode(11, OUTPUT);  
  pinMode(9, OUTPUT);  // Timer1 PWM output pin
  pinMode(8, OUTPUT); 
  pinMode(10, OUTPUT); 
  
  // Configure Timer1 in CTC mode with 1:1 prescaler
  TCCR1A = (1 << COM1A0) | (1 << COM1B0);   // Toggle OC1A and B on match          
  TCCR1B = (1 << WGM12) |    // CTC mode
           (1 << CS10);     // No prescaler
  OCR1A = 639;
  OCR1B = 639;

  TCCR2A = (1 << COM2A0) |  
           (0 << WGM20)  |  // CTC
           (1 << WGM21) ;  // CTC
           
  TCCR2B = (0 << WGM22)  |  // CTC
           (1 << CS20);     // No prescaler
  OCR2A = 255; 

  ADMUX = 0x40; // left adjust, adc0, internal vcc
  ADCSRA = 0xed; // turn on adc, ck/32, auto trigger
  ADCSRB = 0x05; // t1 capture for trigger (compare match B)
  DIDR0 = 0x01; // turn off digital inputs for adc0

  sei();  // Enable global interrupts

}

// Minimal ADC ISR - only reads ADC and sets flag
ISR(ADC_vect) {
  // get ADC data
  byte temp1 = ADCL; // you need to fetch the low byte first
  byte temp2 = ADCH; // yes it needs to be done this way
  adc_value = ((temp2 << 8) | temp1); // make a signed 16b value
  new_sample_flag = 1;     // Set flag for main loop
  TIFR1 = (1 << OCF1B);
  // DEBUG
  // adc_data[count] = adc_value;
  // count++; 
  // PORTB ^= 0x01 ; // Setting 8 high
  
  // if (count >= 63){
  //   count_done = 1;
  //   // PORTB &= 0xfe; // Setting 8 low
  //   count = 0; 
  // };

}



void loop() {
  int32_t result_sum = 0;

  for (uint8_t j = 0; j < double_average; j++) {
    // Reset accumulators for each averaging window
    in_phase = 0;
    quadrature = 0;

    // Process 32 samples (8 of each phase)
    for (int i = 0; i < nsamps; i++) {
      while (!new_sample_flag);
      new_sample_flag = 0;
      int16_t sample = adc_value;

      switch (adc_phase) {
        case 0:
          in_phase += sample;
          break;
        case 1:
          quadrature += sample;
          break;
        case 2:
          in_phase -= sample;
          break;
        case 3:
          quadrature -= sample;
          break;
      }

      adc_phase = (adc_phase + 1) % 4;
    }

    // Compute magnitude approximation for this set
    int16_t result = abs(in_phase) + abs(quadrature);
    result_sum += result;
  }

  // Compute and print average
  int16_t averaged_result = result_sum / double_average;
  Serial.println(averaged_result);
}
