const int LEE_SCL = 2; // puerto de entrada para leer el estado de la líneaSCL
const int LEE_SDA = 3; // puerto de entrada para leer el estado de la líneaSDA
const int ESC_SCL = 4; // puerto de salida para escribir el valor de la línea
const int ESC_SDA = 5; // puerto de salida para escribir el valor de la líneaSDA

//Inicializar variables
int dato, direccion;

void setup() {
  //Iniciar comunicacion con el monitor serial
  Serial.begin(9600);

  // Inicialización de los terminales de entrada
  pinMode(LEE_SDA,INPUT);
  pinMode(LEE_SCL,INPUT);
  // Inicialización de los terminales de salida
  pinMode(ESC_SDA,OUTPUT);
  pinMode(ESC_SCL,OUTPUT);
  //Se Asegura de no intervenir en el bus poniendo SDA y SCL a 1,1
  digitalWrite(ESC_SDA,HIGH);
  digitalWrite(ESC_SCL,HIGH);
} 

void START(){
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);  
  //Se espera a que SDA, SCL = 1,1
  while(digitalRead(LEE_SDA) != HIGH || digitalRead(LEE_SCL) != HIGH);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
}

void STOP(){
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
}

void E_BIT_1(){
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
}

void E_BIT_0(){
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, HIGH);
  digitalWrite(ESC_SDA, LOW);
  digitalWrite(ESC_SCL, LOW);
}

int R_BIT(){     //igual que E-ACK
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  // Se espera a que SCL = 1
  while(digitalRead(LEE_SCL) != HIGH);
  int res = digitalRead(LEE_SDA);

 digitalWrite(ESC_SDA, HIGH);
 digitalWrite(ESC_SCL, LOW);
 return res;
}

// Direccion de la memoria M24C01
void ADDRESS(){
  E_BIT_1();
  E_BIT_0();
  E_BIT_1();
  E_BIT_0();
  E_BIT_0();
  E_BIT_0();
  E_BIT_0();
}

void loop() {
  inico:
  Serial.println("*******************************************************************************");
  Serial.println("Seleccione una de las siguientes opciones: ");
  Serial.println("1. Guardar un dato en cualquier direccionrecion de memoria del dispositivo M24C01");
  Serial.println("2. Leer una posicion de memoria del M24C01");
  Serial.println("3. Inicializar toda la memoria del M24C01 a un valor");
  Serial.println("4. Mostrar el contenido de los 128 bytes del M24C01");
  Serial.println("5. Inicializar toda la memoria del M24C01 a un valor (PageWrite)");
  Serial.println("6. Mostrar el contenido de los 128 bytes del M24C01(Sequential Read)");
  Serial.println("*******************************************************************************\n");

  while (Serial.available() == 0); // Se espera a que se teclee un dato
  int eleccion = Serial.parseInt(); // Se guarda en la variable eleccion el dato que ha sido seleccionado
  // Error si num < 1 o 6 > num
  if (eleccion < 1 || eleccion > 6) {
    Serial.println("LA OPCION INTRODUCIDA ES INCORRECTA\n");
    goto inico;
  } 
  
  switch (eleccion) {
    case 1:
    PedirDireccion();
    PedirDato();
    Serial.println();    
    // Escribimos en memoria
    ByteWrite(direccion, dato); 
    break;

    case 2:
    PedirDireccion();
    Serial.print("En esta posicion de memoria hay un: ");    
    //leemos el dato en la direccion de memoria paada por parametro
    Serial.println(Random_Address_Read(direccion));  
    Serial.println();
    break;

    case 3:
    PedirDato();
    Serial.println();   
    // Llenamos toda la memoria con el dato pasado por parametro
    Rellenar_Memoria(dato); 
    break;

    case 4:
    // Leemos todas las posiciones de memoria
    Leer_Todo();
    Serial.println();
    break;

    case 5:
    PedirDato();
    Serial.println();
    // Llenamos toda la memoria
    Rellenar_Mem_Pagina(dato); 
    break;

    case 6:
    // Se envia la posicion 0 para poder asi leer toda la memoria
    Sequential_Read(0);
    Serial.println();
    break;
  }
}

// Metodo que pasa a bits el dato pasado por parametro
void Pasar_a_bit(byte dato){
  for(int i = 7; i >= 0; i--){
    if (bitRead(dato, i) == 1){
      E_BIT_1();
    }else{
      E_BIT_0();
    }
  }
}

// Metodo que los bits a byte
byte Pasar_a_byte(){
  byte dato = 0;
  for(int i = 7; i >= 0; i--){
    bitWrite(dato, i, R_BIT());
  }
  return dato;
}

// Metodo para escribir un byte en una posición de memoria
void ByteWrite(byte direccion, byte dato){
  volver:
  START(); // Comienza
  ADDRESS(); // Direccion del dispositivo
  E_BIT_0(); // WRITE
  if (R_BIT() != 0) goto volver; // ACK

  //Se envia la direccion
  Pasar_a_bit(direccion);
  //Se espera por la señal ACK
  while(R_BIT() != 0);
  //Se envia el byte
  Pasar_a_bit(dato); 
  //Se espera por la señal ACK
  while(R_BIT() != 0);
  STOP(); //Fin de la operacion
}

// Metodo para escribir 8 bytes en memoria
void PageWrite(byte direccion, byte dato){
  volver:
  START(); // Empezamos
  ADDRESS(); // Direcciondispositivo
  E_BIT_0(); // WRITE
  if (R_BIT() != 0) goto volver; // ACK

  //Enviamos direccion
  Pasar_a_bit(direccion);
  // Esperamos ACK
  while(R_BIT() != 0);
  //Enviamos 8 bytes
  for(int i = 0; i < 8; i++){
    Pasar_a_bit(dato);
    // Esperamos ACK
    while(R_BIT() != 0);
  }
  STOP();
}

// Metodo para leer en una posicion de memoria
byte Random_Address_Read(byte direccion){
  volver:
  START(); // Empezamos
  ADDRESS(); // Direccion dispositivo
  E_BIT_0(); // WRITE
  if(R_BIT() != 0) goto volver; //ACK
  // Enviamos direccion
  Pasar_a_bit(direccion);
  // Esperamos ACK
  while(R_BIT() != 0);

  START(); // Empezamos de nuevo
  ADDRESS(); // Direccion dispositivo
  E_BIT_1(); // LECTURA
  if (R_BIT() != 0) goto volver;
  // Leemos el byte
  byte res = Pasar_a_byte();
  E_BIT_1(); //NO ACK
  STOP(); // Fin de la operacion
  // Se devuelve el byte
  return res;
}

byte Sequential_Read(byte direccion){
  volver:
  START(); //Comienza
  ADDRESS(); // Direccion del dispositivo
  E_BIT_0(); // WRITE
  if (R_BIT() != 0) goto volver; //ACK
  //SE ENVIA LA DIRECCION
  Pasar_a_bit(direccion);
  // Esperamos por la señal ACK
  while(R_BIT() != 0);

  volver2:
  START(); //Comienza
  ADDRESS(); //Direccion del dispositivo
  E_BIT_1(); // LECTURA
  if (R_BIT() != 0) goto volver2; //ACK 
  int salto = 0;
  // Imprimir toda la memoria
  for(int i = 0; i < 128; i++){   
    Serial.print("   ");
    Serial.print(i);
    Serial.print(": 0x"); 
    Serial.print(Pasar_a_byte(), HEX);
    E_BIT_0();
    salto++;    
    if (salto == 8) {
      Serial.println();
      salto = 0;
    }
  }
  E_BIT_1(); // NO ACK
  STOP(); // Fin 
}

// Metodo que llena toda la memoria con ByteWrite
void Rellenar_Memoria(int dato){
  byte num = byte(dato); // Se pasa a byte el numero introducido
  for(int i = 0; i < 128; i++){ // Se recorre toda la memoria
    ByteWrite(byte(i), num); // Se envia direccion y byte a ByteWrite
  }
}

// Metodo que llena toda la memoria con PageWrite
void Rellenar_Mem_Pagina(int dato){
  byte num = byte(dato); // Pasamos a byte el numero introducido
  for (int i = 0; i < 16; i++) { // Recorremos toda la memoria
    PageWrite(byte(i*8), num); // Enviamos direccion y byte a PageWrite
  }
}

// Metodo que imprime por pantalla los datos guardados en memoria
void Leer_Todo(){
  int salto =0;
  for(int i = 0; i < 16; i++){
    for(int j = 0; j < 8; j++)
    Serial.print("   ");
    Serial.print(i*8+j);
    Serial.print(": 0x");
    Serial.print(Random_Address_Read(i*8), HEX);
    salto++;
    if(salto == 8){
      Serial.println();
      salto = 0;
    }    
  }
}

void PedirDireccion(){
  volver:
  Serial.println("Introduzca una posicion de memoria entre 0 y 127: ");
    // Esperamos a que se teclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    direccion = Serial.parseInt(); 
    // Si no está entre 0 y 127 se vuelve a pedir posicion
    if (direccion < 0 || direccion > 127) {
      Serial.println("POSICION DE MEMORIA INTRODUCIDA ERRONEA!!\n");
      goto volver;
    }
    Serial.print("> La posicion introducida es: ");
    Serial.println(direccion);
}

void PedirDato(){
  volver:
  Serial.println("Introduzca un dato entre 0 y 255: ");
    // Esperamos a que se teclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    dato = Serial.parseInt(); 
    //Si no está entre 0 y 255 se vuelve a pedir dato
    if (dato < 0 || dato > 255) {
      Serial.println("NUMERO INTRODUCIDO INCORRECTO!!\n");
      goto volver;
    } 
    Serial.print("> El dato introducido es: ");
    Serial.println(dato);       
}
