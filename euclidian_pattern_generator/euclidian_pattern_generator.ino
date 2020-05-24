void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  euclidianGenerator(16);
}

void euclidianGenerator(int length)
{
    boolean euclidPatterns[length][length];
    for (int i = 0; i < length; i++)
    {
        boolean pattern[length];
        float stepIncrement = (float)length / (float)(i+1);
        float stepPosition = 0.0;
        int arrayIndex = 0;
        for (int j = 0; j < length; j++)
        {
            euclidPatterns[i][j] = 0;
        }
        for (int k = 0; k <= i; k++)
        {
            euclidPatterns[i][arrayIndex] = 1;
            stepPosition += stepIncrement;
            arrayIndex = round(stepPosition);
        }
    }
    for(int m = 0; m < length; m++){
      for(int n = 0; n < length; n++){
        Serial.print(euclidPatterns[m][n]);
        Serial.print("\t");
      }
      Serial.println();
    }
}

void loop() {
  // put your main code here, to run repeatedly:

}
