import sys
import requests

def init_db(url):
    # Create the MyndSync type
    response = requests.post(url + '/type', json={'name': 'MyndSync', 'description': 'MyndSync cognitive training game',
                                                  'static_properties': {'name': {'type': 'string'}},
                                                  'dynamic_properties': {'nickname': {'type': 'string'},
                                                                         'average_response_time': {'type': 'integer'},
                                                                         'first_click_time': {'type': 'integer'},
                                                                         'helps_nr': {'type': 'integer'},
                                                                         'total_lives_nr': {'type': 'integer'},
                                                                         'retries_nr': {'type': 'integer'},
                                                                         'game_time': {'type': 'string'}}})
    user_type = response.json()
    print(user_type)

if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    init_db(url)