# VR PinScreen
This is a Virtual Reality PinScreen application done in 2006/2007, you can check it out on [YouTube](https://www.youtube.com/watch?v=3d88JjktRXM&index=5&list=PLDC731868379777BF)

[![VR PinScreen](http://img.youtube.com/vi/3d88JjktRXM/0.jpg)](https://www.youtube.com/watch?v=3d88JjktRXM)

### 1 - Estrutura de dados da aplicação
É no directorio Data da aplicação que devem estar presentes todos os ficheiros necessários para a execução da aplicação.

Dentro do directório \VR_Pinscreen_-_bin_with_osg\Data a seguinte estrutura de directórios:

Folder | Description
--- | ---
\Animation | Ficheiros de animação
\Images | Imagens
\Model | Modelos dos pinos
\Scripts | Scripts


### 3 - Aplicação
Quando se lança a aplicação esta no final do processo de inicialização corre o script de arranque:
```
\Data\Scripts\start_up.dps
```
este script carrega uma tela de alfinetes de 100 x 100 sem níveis de detalhe.

Para lançar um comando na consola pressione a tecla '\' e a consola desce no ecra. Se pressionar de novo ela sobe.
Todos os comandos começam com o caracter '/'. (curiosamente o tratamento do teclado do osg é um tanto ou quanto estranho e em alguns computadores a devida tecla não imprime esse caracter, se esse for o caso experimente a tecla '^').

Com a consola descida podemos então começar pelo básico.

Command | Description
--- | ---
/list | Para obter a lista de todos os comandos.
/set <sem parametros> | Para obter a lista de variáveis.
/set <variável> <valor> | Para definir o valor da variável.

Tendo em conta estes primeiros comandos podemos então passar a carregar imagens. Como é obvio a imagem tem de estar presente no directório \Data\Images para ser possível o seu carregamento. A aplicação já disponibiliza uma boa quantidade de imagens para teste.

Vamos começar por carregar o einstein
```
/loadLuminanceImage einstein.jpg
```

Carregamos então a mona_lisa
```
/loadLuminanceImage mona_lisa.jpg
```

Na consola podemos usa a tecla "up arrow" para visitar comandos anteriores.

Podemos então definir um modo diferente de desenhar. Existem 4 modos de desenho (absolute, relative, linear, elastic), para alterar o modo basta definir outro valor na variável ps_draw_mode. Vamos entar mudar para o modo elastico.
```
/set ps_draw_mode linear
```

E agora podemos ver a interpolação linear entre as images (que apartir de agora) forem carregadas.
```
/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
```

Podemos alterar a duração da interpolação linear, alterando a variável ps_linear_duration
```
/set ps_linear_duration 5.0

/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
```
 
Vamos agora testar o modo elastico.
```
/set ps_draw_mode elastic


/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
```

Podemos alterar o comportamento do nosso modelo elástico (modelo fisico de molas Spring and Damper) alterando as seguintes variáveis.

Variable | Description
--- | ---
/set ps_spring_damping 0.5 | Atrito da mola (é o atrito que para impede que a mola fique eternamente a oscilar)
/set ps_spring_stiffness 40 | Rigidez da mola (valores elevados -> menos rigida, valores baixo -> mais rigida)
/set ps_pin_mass 3 | Massa de cada pino

Cada vez que alteramos estas variáveis temos de actualizar o nosso modelo elástico, definindo de novo o ps_draw_mode = elastic
```
/set ps_draw_mode elastic

/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
/loadLuminanceImage einstein.jpg
/loadLuminanceImage mona_lisa.jpg
```

Vamos agora testar o LOD
Para isso vamos construir uma tela com 100 x 100 cubos e com +1 nível de detalhe.

Command | Description
--- | ---
/set ps_pin_name cube.osg | Modelo a usar para cada alfinete.
/set ps_lod 1 | Numero de níveis de detalhe (0 = sem níveis de detalhe).
/set ps_lod_distantce 300 | Distãncia entre cada nível de detalhe.
/set ps_v_spacing 1 | Espaçamento vertical entre pinos (engraçado para criar deformações de aspecto) 
/set ps_h_spacing 1 | Espaçamento horizontal entre pinos (engraçado para criar deformações de aspecto) 
/rebuildPinScreen | Para construir a tela com os novos valores

E agora podemos testar de novo com einstein e mona lisa.

Resta então testar as animações.
Para isso vamos fazer um "reset" à tela, correndo de novo o script de start up
```
/run start_up.dps
```

Vamos reproduzir a animação do trainspotting.
Esta animação está em modo swing, por isso quando acabar é reproduzida ao contrário.
```
/play trainspotting_anim.dan
```

Para parar uma animação:
```
/stop
```

Os modos de desenho também funcionam nas animações. No entanto cada animação tem o instante exacto em que cada frame deve ser exibida.
Vamos então fazer da animação usando um modo de desenho linear. Para ver a diferença vamos reproduzir primeiro em modo absoluto e só depois em modo linear.
```
/play hand_anim.dan

/set ps_draw_mode linear
/play hand_anim.dan

/set ps_draw_mode linear
/play hand_anim.dan
```

De novo podemos jogar com a duração da interpolação linear
```
/set ps_linear_duration 3
```


### 3 - Outras informações
A aplicação já disponibiliza um conjunto de imagens e animações prontas a usar. Estas estão presentes na directório Data.

O tamanho das imagens a usar na tela deve coincidir com o tamanho em pinos da mesma. As imagens de exemplo têm todas 100x100 px.

Para criar animações apartir de videos foi usada a aplicação livre virtualdub - http://www.virtualdub.org/. 

Para auxilixar a criação dos ficheiros de animação foram criados alguns scripts que estão no ficheiro helper_scripts.txt no directório de código fonte.

Um bom ficheiro de exemplo para criar animações é hand_anim.dan.

Enviar dúvidas para rui.filipe.varela@gmail.com





 
 


