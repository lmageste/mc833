use tcp

db.createCollection('course')
db.createCollection('person')
db.createCollection('professor')
db.createCollection('student')

db.person.insert([
{
  _id: 'RA1',
  name: 'Lucas',
  address: 'Rua Tal do Fulano',
  birth: new Date(1994, 3, 20),
  email: 'lucas_email@domain.com'
},
{
  _id: 'RA2',
  name: 'Pablo',
  address: 'Rua Esquerda da Direita',
  birth: new Date(1990, 1, 12),
  email: 'pablo_email@domain.com'
},
{
  _id: 'RA3',
  name: 'Emilia',
  address: 'Rua Cima de Baixo',
  birth: new Date(1988, 7, 7),
  email: 'emilia_email@domain.com'
},
{
  _id: 'RA4',
  name: 'Marcela',
  address: 'Rua Menino do Morro',
  birth: new Date(1997, 4, 4),
  email: 'marcela_email@domain.com'
}
])

db.professor.insert([
{
    _id: 'RA4',
    password: 'senha',
    teaching: [
    'MC833',
    'MC458'
    ]
},
{
    _id: 'RA3',
    password: 'senha',
    teaching: [
    'F429'
    ]
}
])

db.student.insert([
{
    _id: 'RA1',
    enrolled: [
    'MC833',
    'MC458',
    'F429'
    ]
},
{
    _id: 'RA2',
    enrolled: [
    'F429'
    ]
},
{
    _id: 'RA3',
    enrolled: [
    'MC833',
    'MC458'
    ]
}
])

db.course.insert([
{
  _id: 'MC833',
  title: 'Programação de Redes de Computadores',
  ects: '2',
  content: 'Programação utilizando diferentes tecnologias de comunicação: sockets, TCP e UDP, e chamada de método remoto.',
  room: 'CC-302',
  timetable: 'Qui.: 10:00 - 12:00',
  students: [
  {
    idStudent: 'RA1'
  },
  {
    idStudent: 'RA3'
  }
  ],
  idProfessor: 'RA4',
  comments: [
  {
    user: 'RA4',
    message: 'Bem-vindos à disciplina de Laboratório de Redes!',
    date_created: new Date(2018, 2, 10, 9, 15)
  }]
},
{
  _id: 'MC458',
  title: 'Projeto e Análise de Algoritmos I',
  ects: '4',
  content: 'Técnicas de projeto e análise de algoritmos. Ferramental Matemático para Análise de Algoritmos. Projeto de algoritmos por indução. Busca, ordenação e estatísticas de ordem. Programação Dinâmica. Algoritmos Gulosos.',
  room: 'CC-353',
  timetable: 'Seg.: 21:00 - 23:00; Qua.: 19:00 - 21:00',
  students: [
  {
    idStudent: 'RA1'
  },
  {
    idStudent: 'RA3'
  }
  ],
  idProfessor: 'RA4',
  comments: [
  {
    user: 'RA4',
    message: 'Bem-vindos à disciplina de Algoritmos!',
    date_created: new Date(2018, 2, 1, 12, 0)
  }]
},
{
  _id: 'F429',
  title: 'Física Experimental IV',
  ects: '2',
  content: 'Experiências de laboratório sobre: propriedades magnéticas da matéria, correntes alternadas, ondas eletromagnéticas, reflexão e refração da luz, polarização, interferência e difração da luz e introdução à física atômica e nuclear.',
  room: 'LF-44',
  timetable: 'Qui.: 08:00 - 10:00',
  students: [
  {
    idStudent: 'RA1'
  },
  {
    idStudent: 'RA2'
  }
  ],
  idProfessor: 'RA3',
  comments: [
  {
    user: 'RA3',
    message: 'Bem-vindos à disciplina de Física Experimental!',
    date_created: new Date(2018, 2, 22, 18, 34)
  }]
}
])
