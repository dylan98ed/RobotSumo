/*
 * https://www.youtube.com/watch?v=xZ3EehV-Ggk&ab_channel=ManuelBautista
*/

class Motor
{
 private: 
 String nombre;
 int borne1, borne2;

 public:
 void ModoInOut(int b1, int b2)
 {
  pinMode(b1, OUTPUT);
  pinMode(b2, OUTPUT);
 }

 void marcha(int b1,int b2) 
{
  digitalWrite(b1, HIGH); 
  digitalWrite(b2, LOW); 
}

void contramarcha(int b1, int b2) 
{
  digitalWrite(b1, LOW); 
  digitalWrite(b2, HIGH); 
}

void parada(int b1, int b2) 
{
  digitalWrite(b1, LOW); 
  digitalWrite(b2, LOW); 
}

void giroIzq(int b1, int b2, int b3, int b4) 
{
  contramarcha(b1,b2); //motor1 - izq
  marcha(b3,b4); //motor2 - der
}

void giroDer(int b1, int b2, int b3, int b4) 
{
  marcha(b1,b2); //motor1 - izq
  contramarcha(b3,b4); //motor2 - der
}
}
