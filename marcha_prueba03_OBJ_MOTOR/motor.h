class Motor
{
 private: 
 String nombre;
 int borne1;
 int borne2;

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

}
