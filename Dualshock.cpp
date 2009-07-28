/*
 *  Driver de Joystick de Playstation2
 *
 *  Baseado no código por Studio Gyokimae (gyokimae@hotmail.com)
 *  Baseado nas especificações por Curious Inventor (http://curiousinventor.com/guides/ps2)
 *  Código reescrito para menor uso de RAM, melhor desempenho e melhor
 *  adequação à orientação à objetos por
 *     Leandro A. F. Pereira (leandro.pereira@gmail.com)
 *
 *  Licenciado sob a GNU GPL versao 2.
 */

/* Comandos para o driver de joystick de playstation */
static unsigned char cmd_poll[] = { 0x01, 0x42 };
static unsigned char cmd_enter_cfg[] = { 0x01, 0x43, 0x00, 0x01 };
static unsigned char cmd_exit_cfg[] = { 0x01, 0x43, 0x00, 0x00, 0x5A, 0x5A, 0x5A, 0x5A, 0x5A };
static unsigned char cmd_ad_mode[] = { 0x01, 0x44, 0x00, 0x01, 0x00 };

class JoystickController {
  private:
    unsigned char m_clock, m_data, m_command, m_attention, m_ack;
    unsigned char m_padState[10];
    boolean m_isDigital;

    void sendCommand(const unsigned char *msg, unsigned char msg_cnt) {
	unsigned char cmd;

	/* Avisa o controle que ele irá receber um comando */
	 digitalWrite(m_attention, LOW);
	 delayMicroseconds(15);

	/* Envia o comando e armazena a resposta */
	for (unsigned char stateBufferIdx = 0;
	     stateBufferIdx < sizeof(m_padState);
	     stateBufferIdx++) {
	    cmd = (stateBufferIdx < msg_cnt) ? msg[stateBufferIdx] : 0x00;
	    m_padState[stateBufferIdx] = readWriteByte(cmd);

	    delayMicroseconds(15);
	}
	
        /* Avisa o controle que o comando já foi recebido */
	digitalWrite(m_attention, HIGH);
	delay(10);
    }

    unsigned char readWriteByte(unsigned char cmd) {
	int i = 0;
	unsigned char received = 0;

	for (i = 0; i < 8; i++) {
	    /* Escreve o primeiro bit do comando na porta de comando */
	    digitalWrite(m_command, cmd & 0x01);
	    /* Rotaciona o byte de comando para a direita */
	    cmd >>= 1;

	    /* Desce o clock */
	    digitalWrite(m_clock, LOW);
	    delayMicroseconds(1);

	    /* Sobe o clock */
	    digitalWrite(m_clock, HIGH);
	    delayMicroseconds(1);

	    /* Prepara o byte de resposta para receber o bit da porta de dados */
	    received >>= 1;
	    /* Armazena o bit lido da porta de dados no byte de resposta */
	    if (digitalRead(m_data))
		received |= 0x80;
	}

	return received;
    }

    void setMode(unsigned char mode, unsigned char lock) {
	cmd_ad_mode[3] = mode;
	cmd_ad_mode[4] = lock;

	sendCommand(cmd_enter_cfg, sizeof(cmd_enter_cfg));
	sendCommand(cmd_ad_mode, sizeof(cmd_ad_mode));
	sendCommand(cmd_exit_cfg, sizeof(cmd_exit_cfg));
    }

  public:
    void update(void) {
	unsigned char prev0, prev1;

	prev0 = m_padState[3];
	prev1 = m_padState[4];

	sendCommand(cmd_poll, sizeof(cmd_poll));

	m_padState[3] ^= prev0;
	m_padState[4] ^= prev1;
    }

    boolean inline isDigital(void) {
	return m_isDigital;
    }

    void inline modeDigital(boolean lock) {
	m_isDigital = true;
	setMode(0x00, lock ? 0x03 : 0x02);
    }

    void inline modeAnalog(boolean lock) {
	m_isDigital = false;
	setMode(0x01, lock ? 0x03 : 0x02);
    }

    boolean inline turnLeft(void) {
	return m_padState[3] & 1 << 7;
    }

    boolean inline turnRight(void) {
	return m_padState[3] & 1 << 5;
    }

    boolean inline turnUp(void) {
	return m_padState[3] & 1 << 4;
    }

    boolean inline turnDown(void) {
	return m_padState[3] & 1 << 6;
    }

    unsigned char inline analogRightX(void) {
	return m_padState[5];
    }

    unsigned char inline analogRightY(void) {
	return m_padState[6];
    }

    unsigned char inline analogLeftX(void) {
	return m_padState[7];
    }

    unsigned char inline analogLeftY(void) {
	return m_padState[8];
    }

    boolean inline isStartDown(void) {
	return m_padState[3] & 1 << 3;
    }

    boolean inline isSelectDown(void) {
	return m_padState[3] & 1;
    }

    boolean inline isLeftStickDown(void) {
	return m_padState[3] & 1 << 1;
    }

    boolean inline isRightStickDown(void) {
	return m_padState[3] & 1 << 2;
    }

    boolean inline isSquareDown(void) {
	return m_padState[4] & 1 << 7;
    }

    boolean inline isCrossDown(void) {
	return m_padState[4] & 1 << 6;
    }

    boolean inline isCircleDown(void) {
	return m_padState[4] & 1 << 5;
    }

    boolean inline isTriangleDown(void) {
	return m_padState[4] & 1 << 4;
    }

    boolean inline isR1Down(void) {
	return m_padState[4] & 1 << 3;
    }

    boolean inline isL1Down(void) {
	return m_padState[4] & 1 << 2;
    }

    boolean inline isR2Down(void) {
	return m_padState[4] & 1 << 1;
    }

    boolean inline isL2Down(void) {
	return m_padState[4] & 1;
    }

  JoystickController(unsigned char clock, unsigned char data, unsigned char command, unsigned char ack, unsigned char attention):
    m_clock(clock), m_data(data), m_command(command), m_ack(ack),
	m_attention(attention) {
	pinMode(m_clock, OUTPUT);
	pinMode(m_command, OUTPUT);

	pinMode(m_data, INPUT);
	pinMode(m_attention, OUTPUT);
	pinMode(m_ack, INPUT);

	digitalWrite(m_clock, HIGH);
	digitalWrite(m_command, LOW);
	digitalWrite(m_attention, HIGH);

	digitalWrite(m_data, LOW);
	digitalWrite(m_ack, LOW);

	modeAnalog(true);
    }
};


#define JOYSTICK_DATA 7
#define JOYSTICK_CLOCK 4
#define JOYSTICK_ATTN 5
#define JOYSTICK_ACK 3
#define JOYSTICK_CMD 6

JoystickController joystick(JOYSTICK_CLOCK, JOYSTICK_DATA, JOYSTICK_CMD,
			    JOYSTICK_ACK, JOYSTICK_ATTN);

void setup()
{
    Serial.begin(9600);

    joystick.modeDigital(true);
}

void loop()
{
    joystick.update();

    Serial.println(joystick.isStartDown());
}
