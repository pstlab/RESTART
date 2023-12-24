from typing import Any, Text, Dict, List

from rasa_sdk import Action, Tracker
from rasa_sdk.executor import CollectingDispatcher

class ActionStartListening(Action):
    def name(self) -> Text:
        return "action_start_listening"

    def run(self, dispatcher: CollectingDispatcher, tracker: Tracker, domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:
        print("Start listening..")
        dispatcher.utter_custom_json({"action_type": "start_listening"})
        return []

class ActionStopTalking(Action):
    def name(self) -> Text:
        return "action_stop_dialogue"

    def run(self, dispatcher: CollectingDispatcher, tracker: Tracker, domain: Dict[Text, Any]) -> List[Dict[Text, Any]]:
        print("Stop dialogue..")
        dispatcher.utter_custom_json({"action_type": "stop_dialogue"})
        return []
