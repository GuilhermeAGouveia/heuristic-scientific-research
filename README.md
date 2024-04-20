# Algoritmos Genéticos para otimização de funções reais

<!---Esses são exemplos. Veja https://shields.io para outras pessoas ou para personalizar este conjunto de escudos . Você pode querer incluir dependências, status do projeto e informações de licença aqui--->

![GitHub repo size](https://img.shields.io/github/repo-size/GuilhermeAGouveia/real-functions-evolutionary-heuristic?style=for-the-badge)
![GitHub language count](https://img.shields.io/github/languages/count/GuilhermeAGouveia/real-functions-evolutionary-heuristic?style=for-the-badge)
![GitHub forks](https://img.shields.io/github/forks/GuilhermeAGouveia/real-functions-evolutionary-heuristic?style=for-the-badge)
![GitHub open issues](https://img.shields.io/github/issues/GuilhermeAGouveia/real-functions-evolutionary-heuristic?style=for-the-badge)

<!---Comparação do algoritmo genético evolucionário simples com o algoritmo genético island-model (multi-populacional)--->

> O propósito deste projeto é explorar a simulação de conceitos evolucionários, visando a validação de teorias genéticas. Atualmente, o repositório conta com a implementação de diversas meta-heurísticas renomadas, tais como: Algoritmo das Formigas, Otimização por Enxame de Partículas (PSO), Clonagem de Anticorpos, Algoritmo da Evolução Diferencial (DE), Algoritmo do Modelo de Ilhas Evolucionárias e Algoritmo Genético.

## 💻 Pré-requisitos

Antes de começar, verifique se você atendeu aos seguintes requisitos:
<!---Estes são apenas requisitos de exemplo. Adicionar, duplicar ou remover conforme necessário--->
#### Tenha instalado (ordem importante):
* WSL (Windows Subsystem for Linux) [for Windows only] :heavy_check_mark:
* docker v20.10.0+ :heavy_check_mark:
* docker-compose v1.29.0+ :heavy_check_mark:

## 🚀 Getting started

Para instalar o projeto, siga estas etapas:

1. Faça download do repositório remoto
```
git clone https://github.com/GuilhermeAGouveia/real-functions-evolutionary-heuristic.git
```
2. Inicie o container docker com todas as dependencias já instaladas
   - Utilizando apenas docker
  
     - ```make run```
   - Utilizando docker-compose com VScode
     1. Instale a extensão docker no VSCode
     2. Clique com o botão direito em cima do arquivo ```docker-compose.yml```
     3. Selecione "Compose up"
     4. Dentro da seção "Docker" no VSCode, encontre o container "inicient-container", clique com o botão direito e selecione "Attach shell"
   
     


> #### Observações: 
> 1. A primeira vez tende a ser demorada, pois é necessário download de diversar dependências
> 2. Se estiver em uma distro linux, e houver erro de permissão, tente adicionar `sudo` aos comandos `make`, exemplo:
> 
> Ao invés de:
> 
> `make init`
> 
> Tente: 
> 
> `sudo make init`

## 📫 Contribuindo com nosso projeto :D

<!---Se o seu README for longo ou se você tiver algum processo ou etapas específicas que deseja que os contribuidores sigam, considere a criação de um arquivo CONTRIBUTING.md separado--->
Para contribuir com esse projeto, siga estas etapas:

1. Bifurque este repositório.
2. Crie um branch: `git checkout -b <nome_branch>`.
3. Faça suas alterações e confirme-as: `git commit -m '<mensagem_commit>'`
4. Envie para o branch original: `git push origin <nome_branch>`
5. Crie a solicitação de pull dentro do GitHub.

Como alternativa, consulte a documentação do GitHub
em [como criar uma solicitação pull](https://help.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request)
.

## 🤝 Colaboradores

Agradecemos às seguintes pessoas que contribuíram para este projeto:

<table>
  <tr>
    <td align="center">
      <a href="https://github.com/caioreius">
        <img width=100 src="https://avatars.githubusercontent.com/u/87735654?v=4" width="100px;" alt="Foto do Iuri Silva no GitHub"/><br>
        <sub>
          <b>Caio Eduardo</b>
        </sub>
      </a>
    </td>
    <td align="center">
          <a href="https://github.com/GuilhermeAGouveia">
            <img width=100 src="https://avatars.githubusercontent.com/u/81968354?v=4" width="100px;" alt="Foto do Steve Jobs"/><br>
            <sub>
              <b>Guilherme Gouveia</b>
            </sub>
          </a>
     </td>
         <td align="center">
          <a href="https://github.com/ViniciusBastoss">
            <img width=100 src="https://avatars.githubusercontent.com/u/117671262?v=4" width="100px;" alt="Foto do Steve Jobs"/><br>
            <sub>
              <b>Vinicius Bueno Bastos</b>
            </sub>
          </a>
     </td>
     <td align="center">
          <a href="https://github.com/gustavo1902">
            <img width=100 src="https://avatars.githubusercontent.com/u/101591580?v=4"/><br>
            <sub>
              <b>Gustavo Morais</b>
            </sub>
          </a>
        </td>
  </tr>
</table>

## 📝 Licença

Esse projeto está sob licença. Veja o arquivo [LICENSE]() para mais detalhes.

[⬆ Voltar ao topo](#Algoritmos-Genéticos-para-otimização-de-funções-reais)<br>

<!---Fim README.md teste--->
