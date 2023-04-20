// Definición de pines por su función
#define IR_INICIO 6
#define IR_INTERMEDIO 7
#define IR_FINAL 8
#define ESTADO_PAVANCE 9
#define ESTADO_PRETROCESO 10
#define ESTADO_SELECTOR 13

// Inclusión de las librerías que usaremos
#include <Stepper.h>
#include <Ultrasonic.h>
#include <MyDelay.h>

// Declaración de las variables que usaremos en el programa
const int stepsPerRevolution = 100;
Stepper myStepper(stepsPerRevolution, 5,2,3,4);  
bool ir_inicio = 1, ir_intermedio, ir_final, estado_pavance, estado_pretroceso, estado_selector;     
int etapa, etapa_anterior, velocidad = 100, distance, posicion_carro = 0, posicion_objeto, posicion_objeto_anterior = 0;  
Ultrasonic us_sensor1(11, 12);
MyDelay timer1(5000);

void setup() {
    // Declaración de los pines del 6 al 10 y el 13 como entradas
    for(int i = 6; i < 11; i++){
        pinMode(i, INPUT);
    }
    pinMode(ESTADO_SELECTOR, INPUT);
    // Definición de la velocidad del motor
    myStepper.setSpeed(velocidad);
    Serial.begin(9600);
    /* Que el motor se mueva hacia el inicio mientras el sensor IR
    no detecte el carro (Función de posicionamiento) */
    while(ir_inicio == 1){
        myStepper.step(1);
        ir_inicio = digitalRead(IR_INICIO);
    }
    timer1.start();
}

void loop() {
    lecturas();
    grafcet();
    // Modo Manual (Selector inactivo)
    if(estado_selector == 0){
        /* Si se pulsa el pulsador de avance, el motor avanza y 
        la variable que almacena su posición incrementa. */
        if(estado_pavance == 1 && ir_final == 1) {
            myStepper.step(-1);
            posicion_carro++;
        }
        /* Si se pulsa el pulsador de retroceso, el motor retrocede
        y la variable que guarda su posición decrementa. */
        if(estado_pretroceso == 1 && ir_inicio == 1) {
            myStepper.step(1);  
            posicion_carro--;
        }
        Serial.println(posicion_carro);
    } 
    // Modo Automático (Selector activo)
    else{
        posicion_objeto = map(distance, 10, 35, 0, 11176);
        /* Mientras el carro esté más avanzado 
        que el objeto, el carro retrocede. */
        if(posicion_carro >= posicion_objeto + 1000 || posicion_carro <= posicion_objeto - 1000){
            while(posicion_carro >= posicion_objeto){
                myStepper.step(1);  
                posicion_carro--;
                lecturas();
                grafcet();
                posicion_objeto = map(distance, 10, 35, 0, 11176);
                // Volver al modo manual inmediatamente después de conmutar el selector.
                if(estado_selector == 0 || ir_inicio == 0) break;
            }
            /* Mientras que el carro esté más atrasado
            que el objeto, el carro avanza. */
            while(posicion_carro <= posicion_objeto){
                myStepper.step(-1);
                posicion_carro++;
                lecturas();
                grafcet();
                posicion_objeto = map(distance, 10, 35, 0, 11176);
                // Volver al modo manual inmediatamente después de conmutar el selector.
                if(estado_selector == 0 || ir_final == 0) break;
            }
        }
    }
    myStepper.setSpeed(velocidad); 
}

void grafcet(){
    // Condiciones para cambiar a cada una de las 5 etapas.
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
    // Acciones a realizar al entrar a cada etapa.
    switch(etapa){
        case 0:
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
    // Lecturas de todas las entradas (Sensores y pulsadores).
    estado_selector = digitalRead(ESTADO_SELECTOR);
    ir_inicio = digitalRead(IR_INICIO);
    ir_intermedio = digitalRead(IR_INTERMEDIO);
    ir_final = digitalRead(IR_FINAL);
    estado_pavance = digitalRead(ESTADO_PAVANCE);
    estado_pretroceso = digitalRead(ESTADO_PRETROCESO);
    distance = us_sensor1.read();
}
