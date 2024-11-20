import sys
import requests
import time

def init_db(url):
    # Create the User type
    response = requests.post(url + '/type', json={'name': 'User', 'description': 'User type',
                                                  'static_properties': {'name': {'type': 'string'},
                                                                        'gender': {'type': 'symbol', 'values': ['M', 'F']},
                                                                        'cognitive_impairment': {'type': 'symbol', 'values': ['L', 'M', 'H']},
                                                                        'physical_impairment': {'type': 'boolean'},
                                                                        'personality_profile':{'type': 'symbol', 'values': ['extroverted', 'introverted']}},
                                                                        
                                                   'dynamic_properties': {'me': {'type': 'boolean'},
                                                                          'open_mic': {'type': 'boolean'},
                                                                          'text': {'type': 'string'},
                                                                          'helps_nr': {'type': 'integer'}}})
    
    user_type=response.json()
    print(user_type)
    
    # Create the UserRecognition type
    response = requests.post(url + '/type', json={'name': 'UserRecognition', 'description': 'User recognition type',
                                                  'static_properties': {'name': {'type': 'string'}},
                                                  'dynamic_properties': {'user': {'type': 'item', 'type_id': user_type['id']},
                                                                         'user_recognition': {'type': 'boolean'}}})
    recognition_type = response.json()
    print("UserRecognition type created:", recognition_type)

    
    
    # Create some reactive rules
    
    #REGOLA DI SETTING DEL NOME
    response = requests.post(url + '/reactive_rule', json={'name': 'user_name', 'content': '(defrule user_name (User_name (item_id ?user) (name ?name)) => (printout t "User: " ?user ", Name: " ?name crlf) (trigger_intent ?user set_name (create$ name) (create$ ?name)))'})
    user_rule = response.json()
    print(user_rule)
    
    #REGOLA DI SETTING DEL GENERE
    response = requests.post(url + '/reactive_rule', json={'name': 'user_gender', 'content': '(defrule user_gender (User_gender (item_id ?user) (gender ?gender)) => (printout t "User: " ?user ", Gender: " ?gender  crlf) (trigger_intent ?user set_gender (create$ gender) (create$ ?gender)))'})
    user_rule = response.json()
    print(user_rule)
    
    #REGOLA DI SETTING DEL PHYSICAL IMPAIRMENT
    response = requests.post(url + '/reactive_rule', json={'name': 'user_physical_impairment', 'content': '(defrule user_physical_impairment (User_physical_impairment (item_id ?user) (physical_impairment TRUE)) => (trigger_intent ?user set_physical_impairment (create$ physical_impairment) (create$ TRUE)))'})
    user_rule = response.json()
    print(user_rule)

    #REGOLA DI GREETINGS CON RICONOSCIMENTO DELL'UTENTE
    response = requests.post(url + '/reactive_rule', json={'name' : 'user_recognition','content':'(defrule user_recognition (UserRecognition_has_user (item_id ?recognition) (user ?user)) (UserRecognition_has_user_recognition (item_id ?recognition) (user_recognition TRUE)) => (trigger_intent ?user greet))'})
    user_rule = response.json()
    print(user_rule)
    
    
    
    
    
    #REGOLA GENERALE SE L'UTENTE SCRIVE UN TESTO (UTILIZZA RASA (COMPUTE_RESPONSE))
    response = requests.post(url + '/reactive_rule', json={'name': 'user_dialogue', 'content': '(defrule user_dialogue (User_has_me (item_id ?user) (me TRUE) (timestamp ?timestamp)) (User_has_text (item_id ?user) (text ?text) (timestamp ?timestamp)) => (printout t "User: " ?user ", Text: " ?text crlf) (compute_response ?user ?text))'})
    user_rule = response.json()
    print(user_rule)
    
    

    # Create some users
    response = requests.post(url + '/item', json={'type': user_type['id'], 'properties': {'name': 'Alice', 'gender': 'F', 'cognitive_impairment':'L', 'physical_impairment': False, 'personality_profile': 'extroverted'}})
    alice = response.json()
    print(alice)
    response = requests.post(url + '/item', json={'type': user_type['id'], 'properties': {'name': 'Bob', 'gender': 'M', 'cognitive_impairment':'M', 'physical_impairment': False, 'personality_profile': 'introverted'}})
    bob = response.json()
    print(bob)
    response = requests.post(url + '/item', json={'type': user_type['id'], 'properties': {'name': 'Maria', 'gender': 'F','cognitive_impairment':'H', 'physical_impairment': True, 'personality_profile': 'introverted'}})
    maria=response.json()
    print(maria)
    
    
    #Create user recognition item
    response = requests.post(url + '/item', json={'type': recognition_type['id'], 'properties': {'name': 'UserRecognition'}})
    recognition_item = response.json()
    print(recognition_item)
    print('Recognition item ID:', recognition_item['id'])
    response = requests.post(url + '/data/' + recognition_item['id'], json={'user': alice['id'], 'user_recognition': True})
    print(response.status_code)
    response = requests.post(url + '/data/' + recognition_item['id'], json={'user': bob['id'], 'user_recognition': True})
    print(response.status_code)
    response = requests.post(url + '/data/' + recognition_item['id'], json={'user': maria['id'], 'user_recognition': True})
    print(response.status_code)
    
    
    
    with open ('greetings.rddl','r') as file:
        data = file.read()
    response = requests.post(url + '/deliberative_rule', json={'name': 'greetings_del', 'content':data})
    user_rule = response.json()
    print(user_rule)
    with open ('robot.clp','r') as file:
        data = file.read()
    response = requests.post(url + '/reactive_rule', json={'name': 'greetings_react', 'content':data})
    user_rule = response.json()
    print(user_rule)
    
    
    
  
if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    init_db(url)