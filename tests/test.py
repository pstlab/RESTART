import sys
import requests
import logging

logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

handler = logging.StreamHandler(sys.stdout)
handler.setLevel(logging.DEBUG)
logger.addHandler(handler)


def create_items(session: requests.Session, url: str) -> list[str]:
    # Create cognitive domain items
    cognitive_domains = ['Memory', 'Attention', 'ExecutiveFunction', 'General']
    cognitive_domain_ids = []
    for domain in cognitive_domains:
        response = session.post(
            url + '/cognitive_domains', json={'name': domain})
        if response.status_code != 201:
            logger.error(f'Failed to create CognitiveDomain item for {domain}')
            return
        cognitive_domain_ids.append(response.text)

    # Create cognitive test items
    cognitive_tests = [
        {'name': 'MoCa', 'domain': 'General'},
        {'name': 'Matrici_Attentive', 'domain': 'Attention'},
        {'name': 'Trial_Making_Test_A', 'domain': 'ExecutiveFunction'},
        {'name': 'Trial_Making_Test_B', 'domain': 'ExecutiveFunction'},
        {'name': 'Trial_Making_Test_B_A', 'domain': 'ExecutiveFunction'},
        {'name': 'Fluenza_semantica', 'domain': 'ExecutiveFunction'},
        {'name': 'Fluenza_fonologica', 'domain': 'ExecutiveFunction'},
        {'name': 'Modified_Winsconsin_Card_Sorting_Test',
            'domain': 'ExecutiveFunction'},
        {'name': 'Breve_racconto', 'domain': 'Memory'}
    ]
    cognitive_test_ids = []
    for test in cognitive_tests:
        response = session.post(
            url + '/cognitive_tests', json=test)
        if response.status_code != 201:
            logger.error(
                f'Failed to create CognitiveTest item for {test["name"]}')
            return
        cognitive_test_ids.append(response.text)

    # Create exercise type items
    exercise_types = [
        {'name': 'memoria visiva', 'duration': 5, 'domain': 'Memory'},
        {'name': 'Att_Attenzione divisa', 'duration': 5, 'domain': 'Attention'},
        {'name': 'fluenza verbale', 'duration': 5, 'domain': 'ExecutiveFunction'}
    ]
    exercise_type_ids = []
    for exercise in exercise_types:
        response = session.post(
            url + '/cognitive_exercises', json=exercise)
        if response.status_code != 201:
            logger.error(
                f'Failed to create ExerciseType item for {exercise["name"]}')
            return
        exercise_type_ids.append(response.text)

    # Create user items
    response = session.post(
        url + '/users', json={'name': 'User1', 'tests': {'MoCa': 1, 'Matrici_Attentive': 1, 'Trial_Making_Test_A': 1, 'Trial_Making_Test_B': 1, 'Trial_Making_Test_B_A': 1, 'Fluenza_semantica': 1, 'Fluenza_fonologica': 1, 'Modified_Winsconsin_Card_Sorting_Test': 1, 'Breve_racconto': 1}})
    if response.status_code != 201:
        logger.error('Failed to create User item')
        return
    user_id = response.text


if __name__ == '__main__':
    url = sys.argv[1] if len(sys.argv) > 1 else 'http://localhost:8080'
    session = requests.Session()

    create_items(session, url)
