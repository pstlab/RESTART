import sys
import requests

def init_db(url):
    # Get the MyndSync type
    response = requests.get(url + '/type?name=MyndSync')
    myndsync_type = response.json()
    print(myndsync_type)

    # Create the MyndSync item
    response = requests.post(url + '/item', json={'type': myndsync_type['id'], 'properties': {'name': 'MyndSync1'}})
    myndsync_item = response.json()
    print(myndsync_item)

if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    init_db(url)