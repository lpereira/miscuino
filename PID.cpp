/**
 * Controlador PID
 * Autor: Leandro A. F. Pereira <leandro@hardinfo.org>
 * Licenca: GNU GPL versao 2
 *
 * Utiliza PID para calcular o torque do motor:
 * - (P)roporcional
 *   Altera o torque baseado na informação atual, diretamente baseada na
 *   diferença do ângulo atual para o ângulo correto.
 * - (I)ntegral
 *   Altera o torque baseado em informações passadas, e no que já ocorreu.
 * - (D)erivativo
 *   Altera o torque baseado na trajetória futura.
 *
 * O uso da classe consiste-se em:
 * - Conhecer os ganhos P, I, D.
 * - Instanciar o objeto.
 * - Chamar o método update() passando o ângulo e a velocidade angular atuais.
 * - Obter ou cada termo independentemente usando os getters fornecidos, ou
 *   obter a soma deles, usando getPID().
 */
class PIDController {
  private:
    double m_PTerm, m_ITerm, m_DTerm;
    double m_oldError;
    double m_PGain, m_IGain, m_DGain;

  public:
    double inline setPTerm(double p) {
        m_PTerm = p;
    }
    
    double inline setITerm(double i) {
        m_ITerm = i;
    }
    
    double inline setDTerm(double d) {
        m_DTerm = d;
    }
    
    double inline getPTerm(void) {
	return m_PTerm;
    }
    
    double inline getITerm(void) {
	return m_ITerm;
    }

    double inline getDTerm(void) {
	return m_DTerm;
    }

    double getPID(void) {
	return m_PTerm + m_DTerm + m_ITerm;
    }

    void update(double error) {
        /* Calcula os termos proporcionais, integrativos e derivativos */
	m_PTerm = error * m_PGain;
	m_ITerm = (m_ITerm + (error * m_IGain)) * 0.9999;
	m_DTerm = (error - m_oldError) * m_DGain;

        /* Salva o valor do ângulo atual para cálculo do termo derivativo */
	m_oldError = error;
    }

    PIDController(double p_gain, double i_gain, double d_gain):
        m_PGain(p_gain), m_IGain(i_gain),
	m_DGain(d_gain), m_PTerm(0.0f), m_ITerm(0.0f), m_DTerm(0.0f), m_oldError(0.0f) {

    }
};


#define P_GAIN       80.0f	/* Constante Proporcional */
#define I_GAIN       140.0f	/* Constante Integrativa */
#define D_GAIN       0.0000001f	/* Constante Derivativa */
#define W_GAIN       5.00f	/* Constante para Velocidade Angular */

/* Controlador PID (proporcional, integrativo, derivativo, velocidade angular) */
PIDController pid(P_GAIN, I_GAIN, D_GAIN, W_GAIN);


void setup()
{

}

void loop()
{
    double error = 0;
    double output;
    
    /* Calcula as novas constantes PID e F de acordo com o ângulo calculado */
    pid.update(error);

    /* Obtém o torque para ser aplicado no motor */
    output = pid.getPID();
}
