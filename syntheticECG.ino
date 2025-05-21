//GERADOR DE ECG

#include "Arduino.h"

// // Parâmetros de configuração do tempo de geração e período de amostragem

// Tempo de início e fim
const float ti = 0;
const float tf = 4;

const float passo = 0.002041; // Período de amostragem, calculado com base na frequência do PWM gerado. Obs.: passo = 1/980, em que 980Hz é a frequência do sinal PWM no pino 6 a partir da função analogWrite(),
const float multiplicator = 1000000;  // Serve para multiplicar os valores gerados pelo modelo para evitar a perda de informação causada pelo truncamento de duas casas decimais pela função map().

// // Parâmetros para modificação do formato das ondas do ECG sintético

//frequência cardíaca e período do ciclo cardíaco
int HR = 75.0;
float tc = 60.0 / HR;

// Parâmetros correspondentes a (P-, P+, Q, R, S, T-, T+)
// Normal
float theta_i[7] = {((-1.0/3)*PI), ((-1.0/3)*PI), ((-1.0/12)*PI), 0.0, ((1.0/12)*PI), ((1.0/2)*PI), ((1.0/2)*PI)};
float a_i[7] = {0.6, 0.6, -5.0, 30.0, -7.5, 0.375, 0.375};
float b_i[7] = {0.25, 0.25, 0.1, 0.1, 0.1, 0.4, 0.4};
// Fibrilação Atrial
// float theta_i[7] = { (-5.0 / 7) * PI, (-1.0 / 2) * PI, (-1.0 / 4) * PI, 0.0, (1.0 / 30) * PI, (1.0 / 4) * PI, (7.0 / 11) * PI };
// float a_i[7] = { 0.7, 0.9, 0.6, 18.0, -0.1, 0.62, 0.55 };
// float b_i[7] = { 0.12, 0.13, 0.12, 0.1, 0.05, 0.15, 0.17 };

// Variáveis auxiliares
float aux = 0;
int i, j = 0;
float sum = 0;
int pwmValue, pwmValueP, pwmValueQRS, pwmValueT = 0;

float omega, deltaTheta_i = 0;
float maximo, minimo = 0;

// Vetores usados para a geração do ECG. Obs.: z guarda o resultado final do ECG para cada instante de tempo.
float p_wave[2] = { 0.0, 0.0 };
float c_complex[2] = { 0.0, 0.0 };
float t_wave[2] = { 0.0, 0.0 };
float phi[2] = { 0.0, 0.0 };
float z[2] = { 0.0, 0.0 };  // saída ECG final

// Número de repetições necessárias para executar o intervalo de tempo de ti para tf.
const int N = (int)((tf - ti) / passo);

//-----------------------------------------------------------------------------------------------------

float py_mod(float a, float b) {
  return a - b * floor(a / b);
}

//-----------------------------------------------------------------------------------------------------

// Cálculos para geração do ECG sintético a partir dos parâmetros
void SyntheticECGGeneration() {

  for (i = 0; i < N - 1; i++) {

    omega = (2 * PI) / tc;
    phi[1] = py_mod((passo * omega + phi[0]), (2 * PI));

    sum = 0;
    for (j = 0; j < 2; j++) {
      deltaTheta_i = py_mod((phi[0] - theta_i[j]), (2 * PI)) - PI;
      sum += (a_i[j] * deltaTheta_i) * exp(-(pow(deltaTheta_i, 2)) / (2 * (pow(b_i[j], 2))));
    }
    p_wave[1] = passo * (-sum) + p_wave[0];

    sum = 0;
    for (j = 2; j < 5; j++) {
      deltaTheta_i = py_mod((phi[0] - theta_i[j]), (2 * PI)) - PI;
      sum += (a_i[j] * deltaTheta_i) * exp(-(pow(deltaTheta_i, 2)) / (2 * (pow(b_i[j], 2))));
    }
    c_complex[1] = passo * (-sum) + c_complex[0];

    sum = 0;
    for (j = 5; j < 7; j++) {
      deltaTheta_i = py_mod((phi[0] - theta_i[j]), (2 * PI)) - PI;
      sum += (a_i[j] * deltaTheta_i) * exp(-(pow(deltaTheta_i, 2)) / (2 * (pow(b_i[j], 2))));
    }
    t_wave[1] = passo * (-sum) + t_wave[0];

    z[1] = p_wave[1] + c_complex[1] + t_wave[1];

    // Valores máximos e mínimos das amplitudes do PWM
    if (maximo < z[0]) {
      maximo = z[0];
    }
    if (minimo > z[0]) {
      minimo = z[0];
    }

    // Remapeamento dos valores do sinal de ECG sintético, para o intervalo do pwm (0 à 255)
    pwmValue = map(z[0] * multiplicator, minimo * multiplicator, maximo * multiplicator, 0, 255);
    // Valor enviado para o pino de saída que após passar pelo filtro será exibido no osciloscópio
    analogWrite(6, pwmValue);
    // Serial.println(pwmValue);

    // // Pinos com as saídas das ondas características
    pwmValueP = map(p_wave[0] * multiplicator, minimo * multiplicator, maximo * multiplicator, 0, 255);
    analogWrite(5, pwmValueP); // Onda característica P individual

    pwmValueQRS = map(c_complex[0] * multiplicator, minimo * multiplicator, maximo * multiplicator, 0, 255);
    analogWrite(10, pwmValueQRS); // Onda característica Complexo QRS individual

    pwmValueT = map(t_wave[0] * multiplicator, minimo * multiplicator, maximo * multiplicator, 0, 255);
    analogWrite(11, pwmValueT); // Onda característica T individual

    phi[0] = phi[1];
    p_wave[0] = p_wave[1];
    c_complex[0] = c_complex[1];
    t_wave[0] = t_wave[1];
    z[0] = z[1];

    delay(1);
  }
  Serial.println("Maximo: ");
  Serial.println(maximo * multiplicator);
  delay(1);
  Serial.println("Minimo: ");
  Serial.println(minimo * multiplicator);
}

//-----------------------------------------------------------------------------------------------------

void setup() {

  Serial.begin(9600);
  // Definindo saídas que vão para o filtro passa baixa que converte o sinal para analógico
  pinMode(6, OUTPUT);

  pinMode(5, OUTPUT);

  pinMode(10, OUTPUT);

  pinMode(11, OUTPUT);

}

void loop() {

  SyntheticECGGeneration();
}
