import requests
ip = input("Inserire l'indirizzo IP locale del dispositivo: ")
pointsValues = requests.get('http://'+ ip +'/log/datalog.txt').text.split("-")
#pointList = pointsValues.text.split("-")
for pointValues in pointsValues:
    path = "points/point_" + str(pointsValues.index(pointValues)) + ".csv"
    file = open(path,"w",newline = '')
    file.write(pointValues)
    file.close()
input("Salvataggio effettuato, premere invio per uscire")
