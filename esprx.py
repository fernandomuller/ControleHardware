from flask import (
    flash, g, redirect, render_template, request, session, url_for, Flask
)

from mysql.connector import MySQLConnection, Error

from time import strftime, time, sleep
import time

from sched import scheduler
import threading
from datetime import datetime

db_config = {
    'user': 'fernando',
    'password': '123456',
    'host': '127.0.0.1',
    'database': 'mydb',
    'raise_on_warnings': True,
}

SECRET_KEY = 'aula de BCD - string aleatória'

app = Flask(__name__)

app.secret_key = SECRET_KEY


def print_time():
    print('Time: %s' % strftime("%Hh%Mmin%Ss"))


irreg = None

error = None


def verificaIrreg(message):

    global irreg
    with app.app_context():
        while 1:
            time.sleep(30)
            print(message)

            try:
                    g.db = MySQLConnection(**db_config)
                    cursor = g.db.cursor(prepared=True)

                    consulta = "SELECT *,TIMEDIFF(Hora_fim,Hora_inicio) AS horas FROM utiliza WHERE TIMEDIFF(Hora_fim,Hora_inicio)>(4*10000)"
                    print("Realizando consulta...  :")
                    cursor.execute(consulta)
                    rows = cursor.fetchall()

                    for r in rows:
                        print("Irregularidade detectada!!!! Ops")
                        sec = r[6]
                        if (sec.seconds*3600) > 4:
                            irreg = "Irregularidade detectada!"

                    cursor.close()
                    g.db.close()
            except Exception as e:
                    print("Tratando exceção...  :")
                    print(str(e))


t = threading.Thread(target=verificaIrreg, args=("Verificando Irregularidades...",))
t.daemon = True
t.start()



@app.route('/')
def main():
    print_time()

    return render_template('index.html')



@app.route('/utilizaBancada', methods=('GET', 'POST'))
def utilizaBancada():
    if request.method == 'POST':

        # Prints para visualização da recepção dos dados Json
        print("Post identificado!")
        print(request)
        print(request.json)
        print(datetime.fromtimestamp(int(request.json['Hora_inicio'])))  

         
        id_funcionario =  request.json['Funcionario_ID']

        # Verifica tipo de post(inicio ou fim)
        if request.json['Hora_fim'] == "0":
            #Extrai os dados Json 
            dados = (datetime.fromtimestamp(int(request.json['Hora_inicio'])), 
                 request.json['Funcionario_ID'],
                 request.json['Bancada_ID'],
                 request.json['Bancada_Sala_ID'])
            consulta = "INSERT INTO Utiliza(Hora_inicio,Funcionario_ID,Bancada_ID,Bancada_Sala_ID) VALUES (%s,%s,%s,%s)"


        else:
            dados = ( datetime.fromtimestamp(int(request.json['Hora_fim'])) ,id_funcionario )  
            consulta = "update utiliza set Hora_fim=(%s) where Funcionario_ID=(%s) and Hora_fim is null"      

        # Em seguida faz commit no Banco de Dados
        g.db = MySQLConnection(**db_config)
        cursor = g.db.cursor()
        cursor.execute(consulta, dados)
        g.db.commit()
        cursor.close()
        g.db.close()

    return "Ok func!"



if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True)
