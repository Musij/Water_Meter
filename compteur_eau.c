#include <math.h>
#include <SoftwareSerial.h>

int TX_Xbee=7;
int RX_Xbee=8;
SoftwareSerial Xbee(RX_Xbee,TX_Xbee);
int pin_sensor=2;
int pin_sleep=10;
int number=1;
int chrono=0;
volatile int NbTopsFan;            //measuring the rising edges of the signal
int Calc; 
float debit=0; 
int _loop = 0;
float qte=0;

void rpm ()                        //This is the function that the interupt calls
{
  NbTopsFan++;                     //This function measures the rising and falling edge of signal
}                                  //The setup() method runs once, when the sketch starts

void setup() {
  // put your setup code here, to run once:
  pinMode(pin_sensor, INPUT);
  pinMode(pin_sleep, OUTPUT);
  digitalWrite(pin_sleep, HIGH);
  Serial.begin(9600);
  Xbee.begin(9600);  
  attachInterrupt(digitalPinToInterrupt(pin_sensor), rpm, RISING);  //and the interrupt is attached
}



void loop() {
  // put your main code here, to run repeatedly:
  NbTopsFan = 0;                    //Set NbTops to 0 ready for calculations
 sei();                            //Enables interrupts
 delay (1000);                     //Wait 1 second
 cli();                            //Disable interrupts
 Calc = (NbTopsFan * 60 / 7.5);    //(Pulse frequency x 60) / 7.5Q = flow rate in L/hour
 debit=debit+Calc;
 if (Calc > 0) {
    _loop=1;
    chrono++;
    if (chrono>(900*number)) {     // si le temps depasse 15 min
        if (number<=3){
         Serial.print("message n° ");//envoyer un message numéroté
         Serial.print(number);
         Serial.print(": Controler votre installation\r\n");
       }
       if (number>3){  // si le numéro estplus grand que 3
         Serial.print("message n° ");//envoyer un message numéroté
         Serial.print(number);
         Serial.print(": Alerte fuite\r\n"); // Envoyer un message d'alerte
         }
       number++;       // incrémenté le numéro
       
     }
     //Serial.print(Calc);
     //Serial.print(" l/h\r\n");

 }
 if (Calc==0) {
     if(_loop==1){
        digitalWrite(pin_sleep,LOW);
        delay(1000);
        Serial.print ("Temps d'utilisation est de : ");
        Serial.print(chrono);
        Serial.print (" S\r\n");
        Serial.print("Debit moyen = ");
        Serial.print(debit/chrono);
        Serial.print(" l/h\r\n");
        qte=debit/3600;
        qte=ceilf(qte*100)/100;
        Serial.print("la consommation d'eau est de : ");
        Serial.print (qte, 2);
        Serial.print (" litre\r\n");
        Xbee.print("la consommation d'eau est de : ");
        Xbee.print (qte, 2);
        Xbee.print (" litre\r\n");
        delay(1000);
        digitalWrite(pin_sleep,HIGH);
        _loop=0;
        debit=0;
        qte=0;
     }
     chrono=0;            // Initialisation du chrono
     number=1;  // Réinitialisation du nombre

 }
}
  
