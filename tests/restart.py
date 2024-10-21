import sys
import requests

def init_db(url):
    # Create the User type
    response = requests.post(url + '/type', json={'name': 'User', 'description': 'User type',
                                                  'static_properties': {'name': {'type': 'string'},
                                                                        'gender': {'type': 'symbol', 'values': ['M', 'F']}},
                                                   'dynamic_properties': {'me': {'type': 'boolean'},
                                                                          'open_mic': {'type': 'boolean'},
                                                                          'text': {'type': 'string'}}})
    user_type = response.json()
    print(user_type)

    # Create some reactive rules
    response = requests.post(url + '/reactive_rule', json={'name': 'user_name', 'content': '(defrule user_name (User_name (item_id ?user) (name ?name)) => (printout t "User: " ?user ", Name: " ?name crlf) (trigger_intent ?user set_name (create$ name) (create$ ?name)))'})
    user_rule = response.json()
    print(user_rule)

    response = requests.post(url + '/reactive_rule', json={'name': 'user_dialogue', 'content': '(defrule user_dialogue (User_has_me (item_id ?user) (me TRUE) (timestamp ?timestamp)) (User_has_text (item_id ?user) (text ?text) (timestamp ?timestamp)) => (printout t "User: " ?user ", Text: " ?text ", Timestamp: " ?timestamp crlf) (compute_response ?user ?text))'})
    user_rule = response.json()
    print(user_rule)

    # Create some users
    response = requests.post(url + '/item', json={'type': user_type['id'], 'properties': {'name': 'Alice', 'gender': 'F'}})
    alice = response.json()
    print(alice)
    response = requests.post(url + '/item', json={'type': user_type['id'], 'properties': {'name': 'Bob', 'gender': 'M'}})
    bob = response.json()
    print(bob)

if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    init_db(url)