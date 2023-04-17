
#define IR_INICIO 6
#define IR_INTERMEDIO 7
#define IR_FINAL 8
#define ESTADO_PAVANCE 9
#define ESTADO_PRETROCESO 10
#define ESTADO_SELECTOR 13

#include <Stepper.h>
#include <Ultrasonic.h>
#include <MyDelay.h>

const int stepsPerRevolution = 100;
Stepper myStepper(stepsPerRevolution, 5,2,3,4);  
bool ir_inicio = 1, ir_intermedio, ir_final, estado_pavance, estado_pretroceso, estado_selector;     
int etapa, etapa_anterior, velocidad = 100, distance, posicion_carro = 0, posicion_objeto;  
Ultrasonic us_sensor1(11, 12);
MyDelay timer1(1000);

void setup() {
    for(int i = 6; i < 11; i++){
        pinMode(i, INPUT);
    }
    pinMode(ESTADO_SELECTOR, INPUT);
    myStepper.setSpeed(velocidad);
    Serial.begin(9600);
    while(ir_inicio == 1){
        myStepper.step(1);
        ir_inicio = digitalRead(IR_INICIO);
    }
    timer1.start();
}

void loop() {
    lecturas();
    grafcet();
    if(estado_selector == 0){
        if(estado_pavance == 1 && ir_final == 1) {
            myStepper.step(-1);
            posicion_carro++;
        }
        if(estado_pretroceso == 1 && ir_inicio == 1) {
            myStepper.step(1);  
            posicion_carro--;
        }
        Serial.println(posicion_carro);
    } 
    else{
        posicion_objeto = map(distance, 10, 35, 0, 11176);
        while(posicion_carro >= posicion_objeto){
            myStepper.step(1);  
            posicion_carro--;
            lecturas();
            grafcet();
            posicion_objeto = map(distance, 10, 35, 0, 11176);
            if(estado_selector == 0) break;
        }
        while(posicion_carro <= posicion_objeto){
            myStepper.step(-1);
            posicion_carro++;
            lecturas();
            grafcet();
            posicion_objeto = map(distance, 10, 35, 0, 11176);
            if(estado_selector == 0) break;
        }
    }
    myStepper.setSpeed(velocidad); 
}

void grafcet(){
    if(ir_inicio == 0) etapa = 0;
    if((etapa == 0 && estado_pavance == 1) || (etapa == 2 && estado_pretroceso == 1)) etapa = 1;
    if((etapa == 1 || etapa == 3) && ir_intermedio == 0) etapa = 2;
    if((etapa == 2 && estado_pavance == 1) || (etapa == 4 && estado_pretroceso == 1)) etapa = 3;
    if(etapa == 3 && ir_final == 0) etapa = 4;
    if(etapa != etapa_anterior){
        Serial.print("etapa: ");
        Serial.println(etapa);
        etapa_anterior = etapa;
    }
    switch(etapa){
        case 0:
            //posicion_carro = 0;
            break;
        case 1:
            if(velocidad != 60) velocidad = 60;
            break;
        case 2:

            break;
        case 3:
            if(velocidad != 100) velocidad = 100;
            break;
        case 4:
            posicion_carro = 11176;
            break;
        default:
            grafcet();
            break;
    }
}

void lecturas(){
    estado_selector = digitalRead(ESTADO_SELECTOR);
    ir_inicio = digitalRead(IR_INICIO);
    ir_intermedio = digitalRead(IR_INTERMEDIO);
    ir_final = digitalRead(IR_FINAL);
    estado_pavance = digitalRead(ESTADO_PAVANCE);
    estado_pretroceso = digitalRead(ESTADO_PRETROCESO);
    if(timer1.update()) distance = us_sensor1.read();
}