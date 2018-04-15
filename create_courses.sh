use tcp

db.createCollection('course')
db.createCollection('person')
db.createCollection('professor')
db.createCollection('student')

db.person.insert([
{
  _id: '1',
  name: 'Lucas',
  address: 'Rua Tal do Fulano',
  birth: new Date(1994, 3, 20),
  email: 'lucas_email@domain.com'
},
{
  _id: '2',
  name: 'Pablo',
  address: 'Rua Esquerda da Direita',
  birth: new Date(1990, 1, 12),
  email: 'pablo_email@domain.com'
},
{
  _id: '3',
  name: 'Emilia',
  address: 'Rua Cima de Baixo',
  birth: new Date(1988, 7, 7),
  email: 'emilia_email@domain.com'
},
{
  _id: '4',
  name: 'Marcela',
  address: 'Rua Menino do Morro',
  birth: new Date(1997, 4, 4),
  email: 'marcela_email@domain.com'
},
{
  _id: '5',
  name: 'Eric',
  address: 'Rua Monteiro Lobato, 12',
  birth: new Date(1994, 4, 4),
  email: 'eric_email@domain.com'
},
{
  _id: '6',
  name: 'Caio Shibuya Carvalho',
  address: 'República Amnésia',
  birth: new Date(1993, 9, 20),
  email: 'caio_shibuya@domain.com'
},
{
  _id: '7',
  name: 'Bruno',
  address: 'Rua Galvão Bueno',
  birth: new Date(1992, 5, 13),
  email: 'bruno_silva@domain.com'
},
{
  _id: '8',
  name: 'Arthur',
  address: 'Rua Guimarães Rosa, 49',
  birth: new Date(1994, 1, 18),
  email: 'arthur_carvalho@domain.com'
},
{
  _id: '9',
  name: 'Jéssica',
  address: 'Rua Machado de Assis, 32',
  birth: new Date(1999, 3, 4),
  email: 'jessica_hoje@domain.com'
}
])

db.professor.insert([
{
    _id: '1011',
    name: 'Pedro Jussieu de Rezende',
    password: 'senha',
    teaching: [
    'MC833',
    'MC458'
    ]
},
{
    _id: '1012',
    name: 'Felippe Alexandre Silva Barbosa',
    password: 'senha',
    teaching: [
    'F429'
    ]
},
{
    _id: '1013',
    name: 'Leandro Aparecido Villas',
    password: 'senha',
    teaching: [
    'MC019'
    ]
},
{
    _id: '1014',
    name: 'Ricardo Edgard Caceffo',
    password: 'senha',
    teaching: [
    'MC102'
    ]
},
{
    _id: '1015',
    name: 'Walmir De Freitas Filho',
    password: 'senha',
    teaching: [
    'ET620'
    ]
}
])

db.student.insert([
{
    _id: '1',
    enrolled: [
    'MC833',
    'MC458',
    'F429'
    ]
},
{
    _id: '2',
    enrolled: [
    'F429'
    ]
},
{
    _id: '3',
    enrolled: [
    'MC833',
    'MC458'
    ]
},
{
    _id: '4',
    enrolled: [
    'MC019'
    ]
},
{
    _id: '5',
    enrolled: [
    'MC019'
    ]
},
{
    _id: '6',
    enrolled: [
    'ET620'
    ]
},
{
    _id: '7',
    enrolled: [
    'MC102'
    ]
},
{
    _id: '8',
    enrolled: [
    'MC102'
    ]
},
{
    _id: '9',
    enrolled: [
    'MC102'
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
    idStudent: '1'
  },
  {
    idStudent: '3'
  }
  ],
  idProfessor: '1011',
  comments: [
  {
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
    idStudent: '1'
  },
  {
    idStudent: '3'
  }
  ],
  idProfessor: '1011',
  comments: [
  {
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
    idStudent: '1'
  },
  {
    idStudent: '2'
  }
  ],
  idProfessor: '1012',
  comments: [
  {
    message: 'Bem-vindos à disciplina de Física Experimental!',
    date_created: new Date(2018, 2, 22, 18, 34)
  }]
},
{
  _id: 'MC019',
  title: 'Estágio Supervisionado em Ciência Computação',
  ects: '12',
  content: 'Trabalho prático que seja válido de acordo com normas internas definidas pelo Instituto de Computação.',
  room: '-',
  timetable: '-',
  students: [
  {
    idStudent: '4'
  },
  {
    idStudent: '5'
  }
  ],
  idProfessor: '1013',
  comments: [
  ]
},
{
  _id: 'MC102',
  title: 'Algoritmos e Programação de Computadores',
  ects: '6',
  content: 'Conceitos básicos de organização de computadores. Construção de algoritmos e sua representação em pseudocódigo e linguagens de alto nível. Desenvolvimento sistemático e implementação de programas. Estruturação, depuração, testes e documentação de programas. Resolução de problemas.',
  room: 'CB04',
  timetable: 'Seg.: 14:00 - 16:00; Qua.: 14:00 - 16:00; Sex.: 14:00 - 16:00',
  students: [
  {
    idStudent: '7'
  },
  {
    idStudent: '8'
  },
  {
    idStudent: '9'
  }
  ],
  idProfessor: '1014',
  comments: [
  ]
},
{
  _id: 'ET620',
  title: 'Máquinas Elétricas',
  ects: '4',
  content: 'Máquina de Corrente Contínua. Máquina Síncrona. Máquina de Indução. Motores de Potência Fracionária e Subfracionária.',
  room: 'FE02',
  timetable: 'Qua.: 19:00 - 20:00; Sex.: 21:00 - 22:00',
  students: [
  {
    idStudent: '6'
  }
  ],
  idProfessor: '1015',
  comments: [
  ]
}
])
