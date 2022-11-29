#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <time.h>
#include <stdlib.h>

//VARIAVEIS DO PROJETO

const float FPS = 60;

int VIDA = 3;
int PONTOS = 0;

const int SCREEN_W = 960;
const int SCREEN_H = 540;

const int NAVE_W = 100;
const int NAVE_Y = 50;

const int ENEMY_W = 60;
const int ENEMY_H = 50;

const int DMG_H = 60;

enum IDS { ESTRELA };
bool keys[] = { false, false };
enum KEYS { ESCAPE, SPACE };

ALLEGRO_AUDIO_STREAM* musica = NULL;
ALLEGRO_SAMPLE* sample = NULL;
ALLEGRO_SAMPLE* trilha = NULL;
ALLEGRO_SAMPLE_INSTANCE* inst_trilha = NULL;

bool tocar_trilha = false;


void draw_scenario() {

	//deixa o background preto
	al_clear_to_color(al_map_rgb(0, 0, 0));

	//desenha o retangulo de colisao / se um inimigo encostar perde o jogo		
	al_draw_filled_rectangle(0, SCREEN_H - DMG_H, SCREEN_W, SCREEN_H,
		al_map_rgb(0, 0, 0));

}

//struct do projetil 
typedef struct projetil {

	int id;
	int x;
	int y;
	int velocidade;
	bool ativo;

}projetil;

//struct da nave
typedef struct Nave {
	float x, y;
	float vel;
	int dir, esq, borda_x, borda_y;
	ALLEGRO_COLOR cor;

} Nave;
//struct do inimigo/obstaculo
typedef struct Enemy {
	float x, y;
	float x_vel, y_vel, borda_x, borda_y;
	bool ativo;
	ALLEGRO_COLOR cor;

}Enemy;
//struct das estrelas
typedef struct Estrelas {

	int ID;
	int x;
	int y;
	int velocidade;

}Estrelas;


typedef struct Conta {
	int x;
	int y;
	int result;
	int escolha;
}Conta;

void initConta(Conta* conta) {
	conta->x = 0;
	conta->y = 0;
	conta->result = 0;
	conta->escolha = 0;
}

void gerarNum(Conta* conta) {
	conta->x = rand() % 10 + 1;
	conta->y = rand() % 10 + 1;
	conta->result = conta->x + conta->y;
}


//inicializacao da nave
void initNave(Nave* nave) {

	nave->borda_x = 100;
	nave->borda_y = 50;
	nave->x = SCREEN_W / 2;
	nave->cor = al_map_rgb(0, 0, 255);
	nave->vel = 4;
	nave->dir = 0;
	nave->esq = 0;

}
//inicializacao do inimigo
void initEnemy(Enemy* enemy) {
	enemy->borda_x = 60;
	enemy->borda_y = 50;
	enemy->x = 10;
	enemy->y = 10;
	enemy->x_vel = 3;
	enemy->y_vel = 1;
	enemy->cor = al_map_rgb(100, 150, 150);
}


//desenho do inimigo 

void draw_enemy(Enemy enemy, ALLEGRO_BITMAP* meteoro) {
	al_draw_bitmap(meteoro, enemy.x, enemy.y, enemy.x + ENEMY_W, enemy.y + ENEMY_H, enemy.cor);
	/*al_draw_filled_rectangle(enemy.x, enemy.y, enemy.x + ENEMY_W, enemy.y + ENEMY_H, enemy.cor);*/

}

//desenho da nave (mudar pra um sprite depois)
void draw_nave(Nave nave, ALLEGRO_BITMAP* navezinha) {
	float y_base = SCREEN_H - DMG_H;

	al_draw_bitmap(navezinha, nave.x - 50, SCREEN_H - 130, 0, 0);

	//hitbox colisao da nave com o inimigo
	/*al_draw_filled_rectangle(nave.x - NAVE_W / 3, y_base - DMG_H, nave.x + NAVE_W / 3, 510,
		al_map_rgb(100, 0, 0));*/
}


void update_nave(Nave* nave) {
	if (nave->dir && nave->x + nave->vel <= SCREEN_W) {
		nave->x += nave->vel;
	}
	if (nave->esq && nave->x - nave->vel >= 0) {
		nave->x -= nave->vel;
	}
}

void update_enemy(Enemy* enemy) {

	if (enemy->x + ENEMY_W + enemy->x_vel > SCREEN_W || enemy->x + enemy->x_vel < 0) {
		enemy->y += enemy->y_vel;
		enemy->x_vel *= -1;
	}

	enemy->x += enemy->x_vel;
	enemy->y += enemy->y_vel;

}

void InitPlano_1(Estrelas estrelas_p1[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p1[i].ID = ESTRELA;
		estrelas_p1[i].y = 5 + rand() % (SCREEN_W + 10);
		estrelas_p1[i].x = 5 + rand() % (SCREEN_H + 10);
		estrelas_p1[i].velocidade = 8;
	}
}
void InitPlano_2(Estrelas estrelas_p2[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p2[i].ID = ESTRELA;
		estrelas_p2[i].y = 5 + rand() % (SCREEN_W + 10);
		estrelas_p2[i].x = 5 + rand() % (SCREEN_H + 10);
		estrelas_p2[i].velocidade = 5;
	}
}
void InitPlano_3(Estrelas estrelas_p3[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p3[i].ID = ESTRELA;
		estrelas_p3[i].y = 5 + rand() % (SCREEN_W + 10);
		estrelas_p3[i].x = 5 + rand() % (SCREEN_H + 10);
		estrelas_p3[i].velocidade = 1;
	}
}
void AtualizarPlano_1(Estrelas estrelas_p1[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p1[i].x -= estrelas_p1[i].velocidade;

		if (estrelas_p1[i].x < 0) {
			estrelas_p1[i].x = SCREEN_H;
		}
	}
}
void AtualizarPlano_2(Estrelas estrelas_p2[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p2[i].x -= estrelas_p2[i].velocidade; 

		if (estrelas_p2[i].x < 0) {
			estrelas_p2[i].x = SCREEN_H;
		}
	}
}
void AtualizarPlano_3(Estrelas estrelas_p3[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		estrelas_p3[i].x -= estrelas_p3[i].velocidade;

		if (estrelas_p3[i].x < 0) {
			estrelas_p3[i].y = SCREEN_H;
		}
	}
}
void DesenhaPlano_1(Estrelas estrelas_p1[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		al_draw_pixel(estrelas_p1[i].y, estrelas_p1[i].x, al_map_rgb(255, 255, 255));
	}
}
void DesenhaPlano_2(Estrelas estrelas_p2[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		al_draw_pixel(estrelas_p2[i].y, estrelas_p2[i].x, al_map_rgb(255, 255, 255));
	}
}
void DesenhaPlano_3(Estrelas estrelas_p3[], int tamanho) {
	for (int i = 0; i < tamanho; i++) {
		al_draw_pixel(estrelas_p3[i].y, estrelas_p3[i].x, al_map_rgb(255, 255, 255));
	}
}


int CometaColidido(Enemy* enemy, Nave* nave) {
	if (nave->x - NAVE_W / 3 < enemy->x &&
		SCREEN_H - DMG_H - 120 < enemy->y &&
		nave->x + NAVE_W / 3 > enemy->x + ENEMY_W &&
		510 > enemy->y + ENEMY_H) {
		return 1;

	}
	return 0;
}
void colisaoPiso(Enemy* enemy) {
	Conta conta;
	if (enemy->y + ENEMY_H > SCREEN_H - DMG_H) {
		VIDA--;
		enemy->x = rand() % SCREEN_W;
		enemy->y = 10;

	}

}

int perdeu() {
	if (VIDA == 0) {
		return 3;
	}
}

int main() {
	enum IDS { ESTRELA };

	int gerarEscolha;
	gerarEscolha = rand() % 3 + 1;
	int gerarRand1, gerarRand2, gerarRand3;
	gerarRand1 = rand() % 10 + 1;
	gerarRand2 = rand() % 10 + 1;
	gerarRand3 = rand() % 10 + 1;


	ALLEGRO_DISPLAY* display = NULL;
	ALLEGRO_EVENT_QUEUE* event_queue = NULL;
	ALLEGRO_TIMER* timer = NULL;
	ALLEGRO_BITMAP* navezinha = NULL;
	ALLEGRO_BITMAP* meteoro = NULL;
	ALLEGRO_BITMAP* menu = NULL;
	ALLEGRO_BITMAP* ins = NULL;
	ALLEGRO_BITMAP* fim = NULL;


	//inicializa o Allegro
	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	//inicializa o módulo de primitivas do Allegro
	if (!al_init_primitives_addon()) {
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}

	//instala o audio 

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(10);

	trilha = al_load_sample("trilha.wav");

	inst_trilha = al_create_sample_instance(trilha);

	al_attach_sample_instance_to_mixer(inst_trilha, al_get_default_mixer());

	al_set_sample_instance_playmode(inst_trilha, ALLEGRO_PLAYMODE_LOOP);

	al_get_sample_instance_gain(inst_trilha);

	al_play_sample_instance(inst_trilha);

	//cria um temporizador que incrementa uma unidade a cada 1.0/FPS segundos
	timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	//cria uma tela com dimensoes de SCREEN_W, SCREEN_H pixels
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	//instala o teclado
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to install keyboard!\n");
		return -1;
	}

	//AUDIOS
	sample = al_load_sample("palmas.wav");

	//inicializa o modulo allegro que carrega as fontes
	al_init_font_addon();
	al_init_ttf_addon();


	//inicializa o modulo que carrega as imagens
	al_init_image_addon();

	// INICIALIZAÇÃO DE OBJETOS

	const int NUM_ESTRELAS = 100;

	Estrelas estrelas_p1[100];
	Estrelas estrelas_p2[100];
	Estrelas estrelas_p3[100];

	//carrega a imagem
	navezinha = al_load_bitmap("spaceship.png");
	meteoro = al_load_bitmap("metheor.png");
	menu = al_load_bitmap("telestia1.png");
	ins = al_load_bitmap("ins.png");
	fim = al_load_bitmap("gameover.png");

	//cria a fila de eventos
	event_queue = al_create_event_queue();
	if (!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}

	//registra na fila os eventos de tela (ex: clicar no X na janela)
	al_register_event_source(event_queue, al_get_display_event_source(display));
	//registra na fila os eventos de tempo: quando o tempo altera de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra na fila os eventos de teclado (ex: pressionar uma tecla)
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_flip_display();


	Nave nave;
	initNave(&nave);

	Enemy enemy;
	initEnemy(&enemy);

	Conta conta;
	initConta(&conta);
	gerarNum(&conta);

	ALLEGRO_FONT* fonte = al_load_font("Minecraft.ttf", 25, NULL);
	InitPlano_1(estrelas_p1, 100);
	InitPlano_2(estrelas_p2, 100);
	InitPlano_3(estrelas_p3, 100);

jogo:
	PONTOS = 0;
	VIDA = 3;
	int playing = 2;
	al_start_timer(timer);
	while (playing == 2) {
		ALLEGRO_EVENT ev;



		al_wait_for_event(event_queue, &ev);

		draw_scenario();


		AtualizarPlano_1(estrelas_p1, 50);
		AtualizarPlano_2(estrelas_p2, 50);
		AtualizarPlano_3(estrelas_p3, 50);

		DesenhaPlano_3(estrelas_p3, 50);
		DesenhaPlano_2(estrelas_p2, 50);
		DesenhaPlano_1(estrelas_p1, 50);

		
		al_draw_bitmap(menu, 0, 0, 0);

		al_flip_display();

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}


		switch (ev.keyboard.keycode) {

		case ALLEGRO_KEY_ESCAPE:
			playing = 0;
			break;
		case ALLEGRO_KEY_ENTER:
			playing = 1;
			break;
		case ALLEGRO_KEY_Z:
			playing = 4;
			break;
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

	}

	while (playing == 4) {
		ALLEGRO_EVENT ev;



		al_wait_for_event(event_queue, &ev);

		draw_scenario();


		AtualizarPlano_1(estrelas_p1, 50);
		AtualizarPlano_2(estrelas_p2, 50);
		AtualizarPlano_3(estrelas_p3, 50);

		DesenhaPlano_3(estrelas_p3, 50);
		DesenhaPlano_2(estrelas_p2, 50);
		DesenhaPlano_1(estrelas_p1, 50);


		al_draw_bitmap(ins, 0, 0, 0);

		al_flip_display();

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}


		switch (ev.keyboard.keycode) {

		case ALLEGRO_KEY_ESCAPE:
			playing = 0;
			break;
		case ALLEGRO_KEY_ENTER:
			playing = 1;
			break;
		case ALLEGRO_KEY_Z:
			playing = 4;
			break;
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

	}


	while (playing == 1) {
		ALLEGRO_EVENT ev;
		//espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		srand(time(NULL));

		draw_scenario();

		AtualizarPlano_1(estrelas_p1, 100);
		AtualizarPlano_2(estrelas_p2, 100);
		AtualizarPlano_3(estrelas_p3, 100);

		DesenhaPlano_3(estrelas_p3, 100);
		DesenhaPlano_2(estrelas_p2, 100);
		DesenhaPlano_1(estrelas_p1, 100);


		if (conta.escolha == conta.result) {
			CometaColidido(&enemy, &nave);

		}


		int gerar = CometaColidido(&enemy, &nave);;
		if (gerar == 1) {
			enemy.x_vel += 0.06;
			enemy.x = rand() % SCREEN_W;
			enemy.y = 10;
			gerarNum(&conta);
			gerarEscolha = rand() % 3 + 1;
			gerarRand1 = rand() % 10 + 1;
			gerarRand2 = rand() % 10 + 1;
			gerarRand3 = rand() % 10 + 1;
			PONTOS += 100;
			conta.escolha = 0;
		}
		colisaoPiso(&enemy);

		if (enemy.x + ENEMY_W > SCREEN_W) {
			enemy.x = rand() % SCREEN_W;
		}

		if (enemy.x < 0) {
			enemy.x = rand() % SCREEN_W;
		}



		//FUNÇÕES INICIAIS



		//se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		if (ev.type == ALLEGRO_EVENT_TIMER) {

			update_nave(&nave);
			update_enemy(&enemy);

			draw_nave(nave, navezinha);
			draw_enemy(enemy, meteoro);

			int keyQ = 0, keyW = 0, keyE = 0;


			if (gerarEscolha == 1)
				keyQ = conta.result;
			else
				keyQ = gerarRand1;
			if (gerarEscolha == 2)
				keyW = conta.result;
			else
				keyW = gerarRand2;
			if (gerarEscolha == 3)
				keyE = conta.result;
			else
				keyE = gerarRand3;


			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2 + 60, 10, ALLEGRO_ALIGN_CENTRE, "%d", conta.escolha);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2, 10, ALLEGRO_ALIGN_CENTRE, "%d + %d = ", conta.x, conta.y);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 900, 510, ALLEGRO_ALIGN_CENTRE, "Q = %d", keyQ);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2, 510, ALLEGRO_ALIGN_CENTRE, "W = %d", keyW);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 60, 510, ALLEGRO_ALIGN_CENTRE, "E = %d", keyE);

			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 70, 30, ALLEGRO_ALIGN_CENTRE, "Vidas: %d", VIDA);
			al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W - 70, 10, ALLEGRO_ALIGN_CENTRE, "Pontos: %d", PONTOS);
			playing = perdeu();


			//atualiza a tela (quando houver algo para mostrar)
			al_flip_display();


			if (al_get_timer_count(timer) % (int)FPS == 0)
				printf("\n%d segundos se passaram...", (int)(al_get_timer_count(timer) / FPS));
		}

		//se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}

		//se o tipo de evento for um pressionar de uma tecla
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);

			switch (ev.keyboard.keycode) {

			case ALLEGRO_KEY_Q:
				if (gerarEscolha == 1)
					conta.escolha = conta.result;
				else
					conta.escolha = gerarRand1;
				break;
			case ALLEGRO_KEY_W:
				if (gerarEscolha == 2)
					conta.escolha = conta.result;
				else
					conta.escolha = gerarRand2;
				break;
			case ALLEGRO_KEY_E:
				if (gerarEscolha == 3)
					conta.escolha = conta.result;
				else
					conta.escolha = gerarRand3;
				break;
			case ALLEGRO_KEY_A:
				nave.esq = 1;
				break;
			case ALLEGRO_KEY_D:
				nave.dir = 1;
				break;
			case ALLEGRO_KEY_ESCAPE:
				keys[ESCAPE] = true;
				break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			//imprime qual tecla foi
			printf("\ncodigo tecla: %d", ev.keyboard.keycode);
			switch (ev.keyboard.keycode) {

			case ALLEGRO_KEY_A:
				nave.esq = 0;
				break;
			case ALLEGRO_KEY_D:
				nave.dir = 0;
				break;
			}

		}


	}//fim do while

	while (playing == 3) {
		ALLEGRO_EVENT ev;

		al_wait_for_event(event_queue, &ev);

		draw_scenario();

		al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2 - 20, SCREEN_H / 2 - 50, ALLEGRO_ALIGN_CENTRE, "VOCE PERDEU, SEUS PONTOS: %d",PONTOS);
		al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2 - 20, SCREEN_H / 2 - 20, ALLEGRO_ALIGN_CENTRE, "APERTE ENTER PARA JOGAR NOVAMENTE");
		al_draw_textf(fonte, al_map_rgb(255, 0, 0), SCREEN_W / 2 - 20, SCREEN_H / 2 - (-10), ALLEGRO_ALIGN_CENTRE, "APERTE ESQ PARA SAIR");

		AtualizarPlano_1(estrelas_p1, 100);
		AtualizarPlano_2(estrelas_p2, 100);
		AtualizarPlano_3(estrelas_p3, 100);

		DesenhaPlano_3(estrelas_p3, 100);
		DesenhaPlano_2(estrelas_p2, 100);
		DesenhaPlano_1(estrelas_p1, 100);

		al_draw_bitmap(fim, 0, 0, 0);

		al_flip_display();


		switch (ev.keyboard.keycode) {

		case ALLEGRO_KEY_ESCAPE:
			playing = 0;
			break;
		case ALLEGRO_KEY_ENTER:
			goto jogo;
			break;
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
	}



	//procedimentos de fim de jogo (fecha a tela, limpa a memoria, etc)

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}