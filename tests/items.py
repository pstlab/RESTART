import sys
import requests
import time

def init_db(url):
    # Get the MyndSync type
    response = requests.get(url + '/type?name=MyndSync')
    myndsync_type = response.json()
    print(myndsync_type)

    # Create the MyndSync item
    response = requests.post(url + '/item', json={'type': myndsync_type['id'], 'properties': {'name': 'MyndSync42'}})
    myndsync_item = response.json()
    print(myndsync_item)
    print('MyndSync item ID:', myndsync_item['id'])

    # Send data to the MyndSync item
    while True:
        time.sleep(10)
        response = requests.post(url + '/data/' + myndsync_item['id'], json={'nickname': '42', 'average_response_time': 5, 'first_click_time': 3, 'helps_nr': 0, 'total_lives_nr': 3, 'retries_nr': 0, 'game_time': 'Sono le ore 12:00'})
        print(response.status_code)

if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    init_db(url)