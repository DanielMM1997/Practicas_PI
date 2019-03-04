/*
 * Tarea 2. Transmisión serie. Añada al programa anterior una nueva funcionalidad consistente en 
 * presentar al usuario un menú en el monitor serie (ventana en el PC) con al menos 5 opciones 
 * para seleccionar; 1) Parpadeo de las unidades; 2) Parpadeo de las decenas; 
 * 3) Parpadeo de las centenas; 4) Parpadeo de las unidades de millar y 
 * 5) Parpadeo secuencial de los 4 dígitos (como tarea1.b). Si lo desea añada más funciones como mejoras.
 */
void setup() {
  //Habilito como salida los 7-segmentos de los 4 displays
  DDRA = B01111111;
  DDRL = B00001111;
  
  //Establece la comunicacion con el Monitor serie
  Serial.begin(9600);
  Serial.println("*******************************************************************************");
  Serial.println("Hola, ¿Que accion desea realizar?");
  Serial.println("1) Hacer papadear las unidades");
  Serial.println("2) Hacer papadear las decenas");
  Serial.println("3) Hacer papadear las centenas");
  Serial.println("4) Hacer papadear las millares");
  Serial.println("5) Hacer papadear los cuatro segmentos sucesivamente");
  Serial.println("6) Music time");
  Serial.println("*******************************************************************************\n"); 
  
}

void loop() {  
  //Esperar a que se teclee un caracter
  while(Serial.available() > 0){
    
    //Guardamos el dato en una variable   
    int teclado = Serial.read();
    
    //Si se pulsa una tecla diferente al 1,2,3,4,5 o 6 salta un error
    if(teclado < 49 || teclado > 54){
      tone(37, 50, 300);
      Serial.println("OPCION INTRODUCIDA, INCORRECTA\n");
    }
  
    switch(teclado){
      //Parpadear las unidades si se pulsa el "1"
      case 49:
        PORTL = B00001110;
        parpadear();
          break;
          
      //Papadear las decenas si se pulsa el "2"
      case 50:
        PORTL = B00001101;
        parpadear();
          break;
          
      //Papadear las centenas si se pulsa el "3"
      case 51:
        PORTL = B00001011;
        parpadear();
          break;
          
      //Papadear las millares si se pulsa el "4"
      case 52:
        PORTL = B00000111;
        parpadear();
          break;
          
      //Hacer papadear los cuatro segmentos sucesivamente si se pulsa el "5"
      case 53:
        PORTL = B00001110;
        parpadear();
        tone(37, 106, 100);
        PORTL = B00001101;
        parpadear();
        tone(37, 106, 100);
        PORTL = B00001011;
        parpadear();
        tone(37, 106, 100);
        PORTL = B00000111;
        parpadear();
        tone(37, 106, 100);
          break;
          
      //Tocar cancion si se pulsa el "6"
      case 54:
        tone(37, 440, 500);
        delay(500);
        tone(37, 440, 500);
        delay(500);
        tone(37, 440, 500);
        delay(500);
        tone(37, 349, 350);
        delay(350);
        tone(37, 523, 150);
        delay(150);
        tone(37, 440, 500);
        delay(500);
        tone(37, 349, 350);
        delay(350);
        tone(37, 523, 150);
        delay(150);
        tone(37, 440, 1000);
        delay(1000);        
        tone(37, 659, 500);
        delay(500);
        tone(37, 659, 500);
        delay(500);
        tone(37, 659, 500);
        delay(500);
        tone(37, 698, 350);
        delay(350);
        tone(37, 523, 150);
        delay(150);
        tone(37, 415, 500);
        delay(500);
        tone(37, 349, 350);
        delay(350);
        tone(37, 523, 150);
        delay(150);
        tone(37, 440, 1000);
        delay(1000);
        tone(37, 880, 500);
        delay(500);
        tone(37, 440, 350);
        delay(350);
        tone(37, 440, 150);
        delay(150);
        tone(37, 880, 500);
        delay(500);
        tone(37, 830, 250);
        delay(250);
        tone(37, 780, 250);
        delay(250);
        tone(37, 740, 150);
        delay(150);
        tone(37, 698, 150);
        delay(150);
        tone(37, 740, 250);
        delay(250);
        tone(37, 455, 250);
        delay(250);
          break;
    }
  }
}

//Metodo para hacer parpadear los displays
void parpadear(){
  PORTA = B01111111;
  delay(500);
  PORTA = B00000000;
  delay(500);
  PORTA = B01111111;
  delay(500);
  PORTA = B00000000;
  delay(500);
  PORTA = B01111111;
  delay(500);
  PORTA = B00000000;
  delay(500);
}
