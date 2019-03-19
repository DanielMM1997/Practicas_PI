const int LEE_SCL = 2; // puerto de entrada para leer el estado de la líneaSCL
const int LEE_SDA = 3; // puerto de entrada para leer el estado de la líneaSDA
const int ESC_SCL = 4; // puerto de salida para escribir el valor de la línea
const int ESC_SDA = 5; // puerto de salida para escribir el valor de la líneaSDA

// Inicializar variables
int dato, direccion;

void setup() {
  // Establece la comunicacion con el Monitor Serie
  Serial.begin(9600);
  // Inicialización de los terminales de entrada
  pinMode(LEE_SDA,INPUT);
  pinMode(LEE_SCL,INPUT);
  // Inicialización de los terminales de salida
  pinMode(ESC_SDA,OUTPUT);
  pinMode(ESC_SCL,OUTPUT);
  // Asegurarse de no intervenir el bus poniendo SDA y SCL a 1,1
  digitalWrite(ESC_SDA,HIGH);
  digitalWrite(ESC_SCL,HIGH);
}

void START(){
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);  
  // Esperar a que SDA, SCL = 1,1
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

int R_BIT(){
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, LOW);
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
  // Esperar a que SCL = 1
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
  inicio:
  Serial.println("*******************************************************************************");
  Serial.println("Seleccione una de las siguientes opciones: ");
  Serial.println("1. Guardar un dato en cualquier dirrecion de memoria del dispositivo M24C01");
  Serial.println("2. Leer una posicion de memoria del M24C01");
  Serial.println("3. Inicializar toda la memoria del M24C01 a un valor");
  Serial.println("4. Mostrar el contenido de los 128 bytes del M24C01");
  Serial.println("5. Inicializar toda la memoria del M24C01 a un valor (PageWrite)");
  Serial.println("6. Mostrar el contenido de los 128 bytes del M24C01(Sequential Read)");
  Serial.println("*******************************************************************************\n");

  while (Serial.available() == 0); // Esperar a que se teclee un dato
  int eleccion = Serial.parseInt(); // Guardamos la opcion elegida
  if (eleccion < 1 || eleccion > 6) {
    Serial.println("OPCION INCORRECTA\n");
    goto inicio;
  } 
  clean();
  
  switch (eleccion) {
    case 1:
    IntroducirDato();      
    IntroducirDireccion();      
    // Escribimos en memoria
    ByteWrite(direccion, dato); 
    break;

    case 2:
    IntroducirDireccion();
    Serial.print("> La posicion introducida es: ");
    Serial.println(direccion);    
    Serial.print("> El numero guardado en memoria es: ");    
    clean();
    //Leemos de memoria
    Serial.println(RANDOM_ADDRESS_READ(direccion));
    Serial.print("\n");
    break;

    case 3:
    IntroducirDato();
    Serial.print("> El dato introducido es: ");
    Serial.println(dato); 
    Serial.print("\n");   
    clean();
    // Llenamos toda la memoria
    fillAllByte(dato); 
    break;

    case 4:
    //Imprimir todos los datos de la memoria
    readAll();
    break;

    case 5:
    IntroducirDato();
    Serial.print("> El dato introducido es: ");  
    Serial.println(dato); 
    Serial.print("\n");   
    clean();
    // Llenamos toda la memoria
    fillAllPage(dato); 
    break;

    case 6:
    // Enviamos la posición 0 para leer toda la memoria
    SEQUENTIAL_RANDOM_READ(0);
    Serial.print("\n");
    break;
  }
}

void IntroducirDato(){
  volver:
  Serial.println("Introduzca un dato entre 0 y 255");
    // Esperamos a que seteclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    dato = Serial.parseInt(); 
    //Si no está entre 0 y 255 se vuelve a pedir dato
    if (dato < 0 || dato > 255) {
      Serial.println("NUMERO INTRODUCIDO INCORRECTO\n");
      goto volver;
    } 
    Serial.print("> El dato introducido es: ");
    Serial.println(dato);         
    clean();
}

void IntroducirDireccion(){
  volver:
  Serial.println("Introduzca una posicion de memoria entre 0 y 127 ");
    // Esperamos a que se teclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    direccion = Serial.parseInt(); 
    // Si no está entre 0 y 128 se vuelve a pedir posicion
    if (direccion < 0 || direccion > 127) {
      Serial.println("POSICION DE MEMORIA INTRODUCIDA ERRONEA\n");
      goto volver;
    }
    Serial.print("> La posicion introducida es: ");
    Serial.println(direccion);    
    clean();
}

// Metodo que pasa a bits el dato pasado por parametro
void Pasar_a_bits(byte dato){
  for(int i = 7; i >= 0; i--){
    if (bitRead(dato, i) == 1){
      E_BIT_1();
    }else{
      E_BIT_0();
    }
  }
}

// Metodo que pasa los bits a byte
byte Pasar_a_byte(){
  byte dato = 0;
  for(int i = 7; i >= 0; i--){
    bitWrite(dato, i, R_BIT());
  }
  return dato;
}

// Metodo para escribir un byte en una posición de memoria
void ByteWrite(byte dir, byte data){
  volver:
  START();       // Empezamos
  ADDRESS();     // Direccion dispositivo
  E_BIT_0();     // WRITE
  if (R_BIT() != 0) 
  goto volver;   // ACK

  // Enviamos direccion
  Pasar_a_bits(dir);
  // Esperamos ACK
  while(R_BIT() != 0);
  // Enviamos byte
  Pasar_a_bits(data);
  // Esperamos ACK
  while(R_BIT() != 0);
  STOP();        //Fin operacion
}

// Metodo para escribir 8 bytes en memoria
void PageWrite(byte dir, byte data){
  volver:
  START();      // Empezamos
  ADDRESS();    // Direcciondispositivo
  E_BIT_0();    // WRITE
  if (R_BIT() != 0) 
  goto volver;  // ACK

  //Enviamos direccion
  Pasar_a_bits(dir);
  // Esperamos ACK
  while(R_BIT() != 0);
  //Enviamos 8 bytes
  for(int i = 0; i < 8; i++){
    Pasar_a_bits(data);
    // Esperamos ACK
    while(R_BIT() != 0);
  }
  STOP();
}

// Metodo para leer en una posicion de memoria
byte RANDOM_ADDRESS_READ(byte dir){
  volver:
  START();      // Empezamos
  ADDRESS();    // Direccion dispositivo
  E_BIT_0();    // WRITE
  if(R_BIT() != 0) 
  goto volver;  //ACK
  // Enviamos direccion
  Pasar_a_bits(dir);
  // Esperamos ACK
  while(R_BIT() != 0);

  START();     // Empezamos de nuevo
  ADDRESS();   // Direccion dispositivo
  E_BIT_1();   // LECTURA
  if (R_BIT() != 0) 
  goto volver;
  // Leemos el byte
  byte res = Pasar_a_byte();
  if (R_BIT() != 1) goto volver;
  E_BIT_1();   //NO ACK
  STOP();      // Fin operacion
  return res;
}

byte SEQUENTIAL_RANDOM_READ(byte dir){
  volver:
  START();     // Empezamos
  ADDRESS();   // Direccion dispositivo
  E_BIT_0();   // WRITE
  if (R_BIT() != 0) 
  goto volver; //ACK
  // ENVIAMOS DIRECCION
  Pasar_a_bits(dir);
  // Esperamos ACK
  while(R_BIT() != 0);

  volver2:
  START();     // Empezamos de nuevo
  ADDRESS();   // Direccion dispositivo
  E_BIT_1();   // READ
  if (R_BIT() != 0) 
  goto volver2; //ACK 
  int l = 0;
  // Imprimir toda la memoria
  for(int i = 0; i < 128; i++){
    Serial.print(Pasar_a_byte(), HEX);
    Serial.print("\n");
    E_BIT_0();
    l++;
    if (l == 8) {
      Serial.print("\n");
      l = 0;
    }
  }
  E_BIT_1();   // NO ACK
  STOP();     
}

// Metodo que llena toda la memoria con ByteWrite
void fillAllByte(int n){
  byte num = byte(n);            // Pasamos a byte el num introducido
  for(int i = 0; i < 128; i++){  // Recorremos toda la memoria
    ByteWrite(byte(i), num);     // enviamos dir y byte a ByteWrite
  }
}

// Metodo que llena toda la memoria con PageWrite
void fillAllPage(int n){
  byte num = byte(n);             // Pasamos a byte el num introducido
  for (int i = 0; i < 16; i++) {  // Recorremos toda la memoria
    PageWrite(byte(i*8), byte(num)); // Enviamos dir y byte a PageWrite
  }
}

// Metodo que imprime por pantalla los datos guardados en memoria
void readAll(){
  for(int j = 0; j < 16; j++){
    for(int i = 0; i < 8; i++){
      Serial.print(RANDOM_ADDRESS_READ(i*8), HEX);
      Serial.print("    ");
  }
  Serial.print("\n");
  }
}

// Método para limpiar el buffer
void clean() {
  while (Serial.available() != 0) {
  Serial.read();
  }
}
