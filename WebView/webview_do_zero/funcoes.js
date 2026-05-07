const fs = require('fs');

function grava()
{
  fs.writeFileSync('file.txt', 'A aula de hoje foi com um aluno \n Assunto: Electron e Node.js');
  alert('Arquivo criado com sucesso!');
}