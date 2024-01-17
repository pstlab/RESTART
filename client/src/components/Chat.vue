<template>
  <v-window-item value="chat" class="fill-height">
    <v-card class="d-flex flex-column fill-height">
      <v-card-text>
        <v-list id="chat-list" class="flex-grow-1 overflow-y-auto" style="max-height: calc(100vh - 190px);">
          <v-list-item v-for="message in messages" :key="message.timestamp"
            :class="message.me ? 'text-right' : 'text-left'">
            <v-chip :color="message.me ? 'primary' : 'secondary'" class="white--text">{{ message.text }}</v-chip>
          </v-list-item>
        </v-list>
      </v-card-text>
      <v-card-actions>
        <v-text-field v-model="message" @keyup.enter="send_message" />
        <v-btn @click="send_message">Send</v-btn>
      </v-card-actions>
    </v-card>
  </v-window-item>
</template>

<script setup>
import { nextTick } from 'vue';
import { reactive, ref } from 'vue';

const messages = reactive([]);
const message = ref('');

const send_message = () => {
  messages.push({ text: message.value, me: true });
  message.value = '';
  nextTick(() => {
    const chat_list = document.getElementById('chat-list');
    chat_list.scrollTop = chat_list.scrollHeight;
  });
};
</script>