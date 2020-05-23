void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  for (int i = 0; i < 16; i++) {
    euclidianGenerator(16, i + 1);
  }
}

void euclidianGenerator(int length, int triggers)
{
  boolean pattern[length];
  float stepIncrement = (float)length / (float)triggers;
  float stepPosition = 0.0;
  int arrayIndex = 0;
  for (int i = 0; i < length; i++) {
    pattern[i] = 0;
  }
  for (int j = 0; j < triggers; j++)
  {
    pattern[arrayIndex] = 1;
    stepPosition += stepIncrement;
    arrayIndex = round(stepPosition);
  }
  Serial.print("boolean euclid_");
  Serial.print(length);
  Serial.print("_");
  Serial.print(triggers);
  Serial.print("[] = {");
  for (int k = 0; k < length; k++) {
    Serial.print(pattern[k]);
    Serial.print(", ");
  }
  Serial.println("};");

}

void loop() {
  // put your main code here, to run repeatedly:

}
