void setup() {
  pinMode(11, OUTPUT);  
  pinMode(9, OUTPUT);  // Timer1 PWM output pin
  
  // Configure Timer1 in CTC mode with 1:1 prescaler
  TCCR1A = (1 << COM1A0);   // Toggle OC1A on match          
  TCCR1B = (1 << WGM12) |    // CTC mode
           (1 << CS10);     // No prescaler
  OCR1A = 831;

  TCCR2A = (1 << COM2A0) |  
           (0 << WGM20)  |  // CTC
           (1 << WGM21) ;  // CTC
           
  TCCR2B = (0 << WGM22)  |  // CTC
           (1 << CS20);     // No prescaler
  OCR2A = 255; 

  ADMUX = 0x40; // left adjust, adc0, internal vcc
  ADCSRA = 0xe5; // turn on adc, ck/32, auto trigger
  ADCSRB = 0x05; // t1 capture for trigger (compare match B)
  DIDR0 = 0x01; // turn off digital inputs for adc0

  sei();  // Enable global interrupts


  // void int nsamps = 20;
  // volatile uint16_t in_phase = 0;
  // volatile uint16_t quadrature = 0;
}

void loop() {
  // measurement
  // 
  
}