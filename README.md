# ECG Sintético com Arduino

Este repositório contém o código responsável pela geração de sinais de ECG (eletrocardiograma) sintético (syntheticECG.ino), implementado em um microcontrolador Arduino. O projeto permite simular um traçado de ECG com base em parâmetros configuráveis, útil para testes ou ensino.

## 🔧 Funcionalidades

- Geração contínua de ECG sintético via sinal analógico (por meio de um filtro passa-baixa e saída do Arduino em PWM).
- Ajuste de parâmetros como frequência cardíaca (HR), amplitude ($a_i$), largura ($b_i$) e posição ($\theta_i$) de cada gaussiana reponsável por representar as ondas do ECG.
- Simulação de um ECG típico de um indivíduo saudável ou com fribrilação atrial (FA).
- Compatível e tastado com Arduino Uno R3.
