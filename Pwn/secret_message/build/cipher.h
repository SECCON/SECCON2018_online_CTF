typedef unsigned long long QWORD;
typedef unsigned int DWORD;
typedef unsigned char BYTE;


/****************************** MACROS ******************************/
#define BLOWFISH_BLOCK_SIZE 8           // Blowfish operates on 8 bytes at a time

/**************************** DATA TYPES ****************************/
typedef unsigned int  WORD;             // 32-bit word, change to "long" for 16-bit machines

typedef struct {
	WORD p[18];
	WORD s[4][256];
} BLOWFISH_KEY;

typedef struct Matrix {
	union {
		double x[16];
		QWORD q[16];
	};

}Matrix;
typedef struct Part {
	union {
		BYTE b[8][8];
	};
}Part;
Matrix QWORD_to_Matrix(QWORD q);
QWORD Matrix_to_QWORD(Matrix m);
class Cipher {
private:
	QWORD key, ROUND;
	Matrix km, km_inv;
	Matrix const_m;
	BLOWFISH_KEY bkey;
	void blowfish_encrypt(const BYTE in[], BYTE out[], const BLOWFISH_KEY *keystruct);
	void blowfish_decrypt(const BYTE in[], BYTE out[], const BLOWFISH_KEY *keystruct);
	Matrix Matrix_mul(Matrix x, Matrix y);
	void blowfish_key_setup(const BYTE user_key[], BLOWFISH_KEY *keystruct, size_t len);
	Part blowfish_enc(Part x);
	Part blowfish_dec(Part x);
	bool Matrix_inv(double m[16], double invOut[16]);
public:

	Cipher();
	bool setkey(QWORD _key);
	Matrix decrypt(Matrix c);
	Matrix encrypt(Matrix p);
};