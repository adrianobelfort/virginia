%% @author grupo03
%% @doc Implementacao de uma rede omega utilizando Erlang

-module(omega).

-export([main/1, transmitter/2, receiver/1, intermediate/4]).
-export([send/3]).
-export([rotateLeft/2,calculateP/1,calculateDestLine/2,isolate/2,getPositionIntermediate/3]).
-export([binaryStringToInteger/1,stringToTuple/1,subtract/3]).

% Funcao principal, recebendo os argumentos da simulacao da rede omega
main(Args) ->
	N = isolate(1, Args),
	Src = isolate(2, Args),
	Dest = isolate(3, Args),

	% Os argumentos sao passados para a funcao send
	send(list_to_integer(N), Src, Dest).

% Dada uma linha de um elemento de saida, retorna a linha do elemento de entrada
calculateDestLine(SrcLine, Total) -> 
	RotatedValue1 = rotateLeft(element(1,calculateP(SrcLine)),Total), 
	RotatedValue2 = rotateLeft(element(2,calculateP(SrcLine)),Total),
	{(trunc(RotatedValue1/2)) + 1, (trunc(RotatedValue2/2)) + 1}.

% Funcao auxiliar para o calculateDestLine, retorna o fio de saida
calculateP(LineNum) ->
	{(LineNum-1)*2,(LineNum-1)*2+1}.

% Isola os elementos de uma lista.
isolate(Position, List) ->
	lists:nth(Position, List).

% Funcao que representa um no transmissor
transmitter(N, Total) ->
	% Recebimento das listas de enderecos
	receive
		[_Transmitters, Intermediates, _Receivers] -> 
			% Calculo da posicao do elemento ao qual o transmissor deve se conectar
			Position = trunc(rotateLeft(N-1, Total)/2)+1,
			% Realizacao da conexao
			Connection = lists:nth(getPositionIntermediate(Position, 1, Total), Intermediates)
	end,
	receive
		% Espera por uma mensagem que deve ser mandada, iniciando a transmissao
		Dest -> 
			io:format("Enviando mensagem a partir do transmissor ~w~n",[Dest]),
			% Envia a mensagem para o endereco armazenado em Connection
			Connection ! Dest
	end.

% Funcao que representa um no receptor
receiver(N) ->
	receive
		% Recebe a mensagem e imprime na tela
		Dest -> 
			io:format("Receptor ~w recebeu mensagem para destino ~w~n", [N-1, Dest])
	end.

% Funcao que representa um no intermediario no caso em que nao estamos no ultimo estagio
intermediate(Line, Column, Total, LastColumn) when Column =/= LastColumn ->
 	receive
 		[_Transmitters, Intermediates, _Receivers] -> 
 			% Calcula as posicoes da lista de nos intermediarios para as quais a mensagem deve ser
 			% roteada. Eh possivel passar para o proximo estagio incrementando o numero da coluna.
			LinePosition = calculateDestLine(Line,Total),
 			Connection1 = lists:nth(getPositionIntermediate(element(1,LinePosition), Column+1, Total), Intermediates),
 			Connection2 = lists:nth(getPositionIntermediate(element(2,LinePosition), Column+1, Total), Intermediates)
 	end,
	receive
		Dest -> 
			% Ao receber uma mensagem, o no intermediario redireciona ela para o proximo estagio
			io:format("Enviando mensagem para destino ~w, do intermediario coluna ~w~w~n", [Dest, Column, Line]),
			redirect(element(Column, Dest), Connection1, Connection2, Dest)
	end;

% Funcao que representa um no intermediario no estagio final
intermediate(Line, Column, Total, LastColumn) when Column == LastColumn ->
 	receive
 		[_Transmitters, _Intermediates, Receivers] -> 
 			% As conexoes para o no intermediario de estagio final sao definidas,
 			% utilizando a lista de enderecos de nos receptores
 			Connection1 = lists:nth(Total - ((Line-1)*2), Receivers),
			Connection2 = lists:nth(Total - ((Line-1)*2 + 1), Receivers)
 	end,
	receive
		Dest -> 
			% O no intermediario no ultimo estagio redireciona a mensagem recebida para o destino
			io:format("Enviando mensagem para destino ~w, do intermediario coluna ~w, linha ~w~n", [Dest, Column, Line]),
			redirect(element(Column, Dest), Connection1, Connection2, Dest)
	end.

% Funcao utilizada para transformar uma string (lista) em uma tupla
stringToTuple(String) ->
	% Tupla obtida a partir da String recebida.
	Tuple = list_to_tuple(String),
	% Funcao utilizada para converter os valores dos caracteres ASCII em valores inteiros.
	subtract(1,Tuple,size(Tuple)).

% Funcao recursiva que realiza subtracao para obter um digito inteiro a partir do valor de
% um caractere ASCII
subtract(Element,Tuple,Size) when Element>Size ->
	Tuple;

subtract(Element,Tuple,Size) when Element =< Size ->
	% Chamada recursiva, onde modificamos o caractere atual ao subtrair 48, obtendo sua
	% representacao em numero inteiro.
	subtract(Element+1,setelement(Element,Tuple,element(Element,Tuple)-48),Size).

% Funcao utilizada para criar os nos transmissores, intermediarios e receptores.
% As listas com os enderecos dos nos tambem sao criadas.
send(N,Src,Dest) ->
	Transmitters = createTransmitters(N, N, []),
	Intermediates = createIntermediates(trunc(N/2), trunc(N/2), trunc(math:log2(N)), [],N),
	Receivers = createReceivers(N, []),

	% Para cada no transmissor na lista, enviamos as listas de enderecos
	lists:foreach(fun(Transmitter) -> Transmitter ! [Transmitters, Intermediates, Receivers] end, Transmitters),
	% O mesmo acontece para cada no intermediario
	lists:foreach(fun(Intermediate) -> Intermediate ! [Transmitters, Intermediates, Receivers] end, Intermediates),
	% Eh enviado para o transmissor recebido como parametro do programa uma mensagem
	% A mensagem eh formada pela identificacao do no de destino
	lists:nth(N - (binaryStringToInteger(Src)),Transmitters) ! stringToTuple(Dest).

% Criacao dos nos transmissores, de maneira recursiva
createTransmitters(N, Total, List) when N == 1 -> lists:append(List, [spawn(omega, transmitter,[N, Total])]);
createTransmitters(N, Total, List) when N > 1 -> createTransmitters(N-1, Total, lists:append(List, [spawn(omega, transmitter,[N, Total])])).

% Criacao dos nos intermediarios, de maneira recursiva
createIntermediates(Lines, _TotalLines, Columns, List, N) when Lines == 1, Columns == 1 ->  lists:append(List, [spawn(omega, intermediate,[Lines, Columns, N, trunc(math:log2(N))])]);
createIntermediates(Lines, TotalLines, Columns, List, N) when Lines > 1, Columns == 1 ->  createIntermediates(Lines-1, TotalLines, Columns, lists:append(List, [spawn(omega, intermediate,[Lines, Columns, N, trunc(math:log2(N))])]),N);
createIntermediates(Lines, TotalLines, Columns, List, N) when Lines == 1, Columns > 1 ->  createIntermediates(TotalLines, TotalLines, Columns-1, lists:append(List, [spawn(omega, intermediate,[Lines, Columns, N, trunc(math:log2(N))])]),N);
createIntermediates(Lines, TotalLines, Columns, List, N) when Lines > 1, Columns > 1 ->  createIntermediates(Lines-1, TotalLines, Columns, lists:append(List, [spawn(omega, intermediate,[Lines, Columns, N, trunc(math:log2(N))])]),N).

% Criacao dos nos receptores, de maneira recursiva
createReceivers(N, List) when N == 1 -> lists:append(List, [spawn(omega, receiver,[N])]);
createReceivers(N, List) when N > 1 -> createReceivers(N-1, lists:append(List, [spawn(omega, receiver,[N])])).

% Dado um numero de linha e um numero de coluna, eh calculada a posicao da lista
% de nos intermediarios onde se encontra o PID do no desejado
getPositionIntermediate(Line, Column, N) -> 
	TotalLines = trunc(N/2),
	TotalColumns = trunc(math:log2(N)),
	(TotalLines * TotalColumns) - ((Column - 1) * TotalLines + Line)+1.

% Funcao utilizada para realizar o bitshift circular a esquerda
rotateLeft(N, Total) ->
	LeftShift = 1,
	RightShift = trunc(math:log2(Total)) - 1,
	Mask = Total - 1,
	(((N bsl LeftShift) bxor (N bsr RightShift)) band Mask).

% Conversao de um numero binario (no formato de string) para inteiro
binaryStringToInteger(String) ->
	lists:nth(1, element(2, io_lib:fread("~2u", String))).

% Funcao de redirecionamento. Caso o valor de controle seja 0,
% a mensagem eh enviada para o Destino 1. Caso contratio, eh enviada
% para o destino 2. O valor de controle eh dado pelo bit do destino na
% posicao referente ao estagio atual
redirect(Val, Dest1, _Dest2, Msg) when Val==0 ->
	Dest1 ! Msg;
redirect(Val, _Dest1, Dest2, Msg) when Val==1 ->
	Dest2 ! Msg.