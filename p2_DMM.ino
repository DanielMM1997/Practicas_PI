const int LEE_SCL = 2; // puerto de entrada para leer el estado de la líneaSCL
const int LEE_SDA = 3; // puerto de entrada para leer el estado de la líneaSDA
const int ESC_SCL = 4; // puerto de salida para escribir el valor de la línea
const int ESC_SDA = 5; // puerto de salida para escribir el valor de la líneaSDA

void setup() {
  //Iniciar comunicacion con el monitor serial
  Serial.begin(9600);

  //Inicialización de los terminales de entrada
  pinMode(LEE_SDA, INPUT);
  pinMode(LEE_SCL, INPUT);
  //Inicialización de los terminales de salida
  pinMode(ESC_SDA, OUTPUT);
  pinMode(ESC_SCL, OUTPUT);
  //Se asegura de no intervenir en el bus poniendo SDA y SCL a 1,1
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);
} 

void START(){
  digitalWrite(ESC_SDA, HIGH);
  digitalWrite(ESC_SCL, HIGH);  
  //Esperar a que SDA, SCL = 1,1
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
  //Esperar a que SCL = 1
  while(digitalRead(LEE_SCL) != HIGH);
  //Guadar el bit introducido
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
  Serial.println("\n\n*******************************************************************************");
  Serial.println("Seleccione una de las siguientes opciones: ");
  Serial.println("1. Guardar un dato en cualquier direccionrecion de memoria del dispositivo M24C01");
  Serial.println("2. Leer una posicion de memoria del M24C01");
  Serial.println("3. Inicializar toda la memoria del M24C01 a un valor");
  Serial.println("4. Mostrar el contenido de los 128 bytes del M24C01");
  Serial.println("5. Inicializar toda la memoria del M24C01 a un valor (PageWrite)");
  Serial.println("6. Mostrar el contenido de los 128 bytes del M24C01(Sequential Read)");
  Serial.println("*******************************************************************************\n");

  // Se espera a que se teclee un dato
  while (Serial.available() == 0); 
  // Se guarda en la variable eleccion la opcion que ha sido seleccionado
  int eleccion = Serial.parseInt(); 
  // Error si num < 1 o 6 > num
  if (eleccion < 1 || eleccion > 6) {
    Serial.println("OPCION INCORRECTA, VUELVA A INTRODUCIR UNA OPCION");
    goto inico;
  }
  clean(); 
  //Inicializar variables
  int dato, direccion;

  switch (eleccion){
    case 1:
    dato = PedirDato();
    direccion = PedirDireccion();
    Serial.print("\nHa introducido un ");
    Serial.print(dato);
    Serial.print(" en la posicion de memoria: ");
    Serial.print(direccion);    
    clean();
    //Escribimos en memoria en una direccion y un dato pasado por parametro
    ByteWrite(direccion, dato); 
    break;

    case 2:
    direccion = PedirDireccion();
    Serial.print("\nEn la posicion de memoria ");
    Serial.print(direccion);
    Serial.print(" hay un: ");    
    clean();
    //Llemos el dato en la direccion de memoria paada por parametro
    Serial.print(Random_Address_Read(direccion));  
    break;

    case 3:
    dato = PedirDato();
    Serial.print("\nHa rellenado toda la memoria con un: ");
    Serial.print(dato);   
    clean();
    //Llenamos toda la memoria con el dato pasado por parametro
    Rellenar_Memoria(dato); 
    break;

    case 4:
    // Leemos todas las posiciones de memoria
    Leer_Todo();
    break;

    case 5:
    dato = PedirDato();
    Serial.print("\nHa rellenado toda la memoria con un: ");
    Serial.print(dato); 
    //Llenamos toda la memoria
    clean();
    Rellenar_Mem_Pagina(dato); 
    break;

    case 6:
    //Enviamos la posicion 0 para leer la memoria desde el principio
    Sequential_Read(0);
    break;
  }
}

//Metodo que pasa a bits el dato pasado por parametro
void Pasar_a_bit(byte data){
  for(int i = 7; i >= 0; i--){
    if (bitRead(data, i) == 1){
      E_BIT_1();
    }else{
      E_BIT_0();
    }
  }
}

//Metodo que los bits a byte
byte Pasar_a_byte(){
  byte data = 0;
  for(int i = 7; i >= 0; i--){
    bitWrite(data, i, R_BIT());
  }
  return data;
}

// Metodo para escribir un byte en una posición de memoria
void ByteWrite(byte address, byte data){
  volver1:
  START();        //Comienza la comunicacion con el maestro
  ADDRESS();      //Direccion del dispositivo
  E_BIT_0();      //ESCRITURA
  if (R_BIT() != 0) goto volver1; // ACK

  //Se envia la direccion
  Pasar_a_bit(address);
  //Se espera por la señal ACK
  while(R_BIT() != 0);
  //Se envia el byte
  Pasar_a_bit(data); 
  //Se espera por la señal ACK
  while(R_BIT() != 0);
  STOP();          //Fin de la operacion
}

//Metodo para escribir 8 bytes en memoria
void PageWrite(byte address, byte data){
  volver2:
  START();         //Empezamos la comunicacion con el maestro
  ADDRESS();       //Direcciondispositivo
  E_BIT_0();       //ESCRITURA
  if (R_BIT() != 0) goto volver2; // ACK

  //Enviamos direccion
  Pasar_a_bit(address);
  // Esperamos ACK
  while(R_BIT() != 0);
  //Enviamos 8 bytes
  for(int i = 0; i < 8; i++){
    Pasar_a_bit(data);
    // Esperamos ACK
    while(R_BIT() != 0);
  }
  STOP();          //Fin de la operacion
}

//Metodo para leer en una posicion de memoria
byte Random_Address_Read(byte address){
  volver3:
  START();        //Empezamos la comunicacion con el maestro
  ADDRESS();      //Direccion dispositivo
  E_BIT_0();      //ESCRITURA
  if(R_BIT() != 0) goto volver3; //ACK
  // Enviamos direccion
  Pasar_a_bit(address);
  // Esperamos ACK
  while(R_BIT() != 0);

  START();        //Empezamos la comunicacion con el maestro
  ADDRESS();      //Direccion dispositivo
  E_BIT_1();      //LECTURA
  if (R_BIT() != 0) goto volver3;
  // Leemos el byte
  byte res = Pasar_a_byte();
  E_BIT_1();      //NO ACK
  STOP();         // Fin de la operacion
  // Se devuelve el byte
  return res;
}

byte Sequential_Read(byte address){
  volver4:
  START();        //Empezamos la comunicacion con el maestro
  ADDRESS();      //Direccion del dispositivo
  E_BIT_0();      //ESCRITURA
  if (R_BIT() != 0) goto volver4; //ACK
  //SE ENVIA LA DIRECCION
  Pasar_a_bit(address);
  // Esperamos por la señal ACK
  while(R_BIT() != 0);

  volver5:
  START();        //Empezamos la comunicacion con el maestro
  ADDRESS();      //Direccion del dispositivo
  E_BIT_1();      //LECTURA
  if (R_BIT() != 0) goto volver5; //ACK 
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
  E_BIT_1();     //NO ACK
  STOP();        //Fin de la operacion
}

// Metodo que llena toda la memoria con ByteWrite
void Rellenar_Memoria(int data){
  //Se recorre toda la memoria
  for(int i = 0; i < 128; i++){     
    //Se rellena toda la memoria con el dato pasado por parametro usando ByteWrite
    ByteWrite(byte(i), byte(data)); 
  }
}

// Metodo que llena toda la memoria con PageWrite
void Rellenar_Mem_Pagina(int data){
  // Recorremos toda la memoria
  for (int i = 0; i < 16; i++){ 
    //Se rellena toda la memoria con el dato pasado por parametro usando PageWrite
    PageWrite(byte(i*8), byte(data)); 
  }
}

// Metodo que imprime por pantalla los datos guardados en memoria
void Leer_Todo(){
  for(int i = 0; i < 16; i++){
    for(int j = 0; j < 8; j++){
      Serial.print("   ");
      Serial.print(i*8+j);
      Serial.print(": 0x");
      Serial.print(Random_Address_Read(i*8+j), HEX);
    }
    Serial.println();    
  }
}

int PedirDireccion(){
  volver6:
  Serial.println("Introduzca una posicion de memoria entre 0 y 127: ");
    // Esperamos a que se teclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    int dir = Serial.parseInt(); 
    // Si no está entre 0 y 127 se vuelve a pedir posicion
    if (dir < 0 || dir > 127){
      Serial.println("POSICION DE MEMORIA INTRODUCIDA ERRONEA!!\n");
      goto volver6;
    }
    clean();
    return dir;
}

int PedirDato(){
  volver7:
  Serial.println("Introduzca un dato entre 0 y 255: ");
    // Esperamos a que se teclee un dato
    while (Serial.available() == 0); 
    // Guardamos el dato
    int data = Serial.parseInt(); 
    //Si no está entre 0 y 255 se vuelve a pedir dato
    if (data < 0 || data > 255){
      Serial.println("NUMERO INTRODUCIDO INCORRECTO!!\n");
      goto volver7;
    } 
    clean(); 
    return data;      
}

// Método para limpiar el buffer
void clean() {
  while (Serial.available() != 0) {
  Serial.read();
  }
}
