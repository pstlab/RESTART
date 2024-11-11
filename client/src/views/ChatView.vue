<template>
  <coco-chat v-if="itm" :item="itm" lang="it-IT" voice="Microsoft Elsa - Italian (Italy)" :key="itm.id" />
</template>

<script setup lang="ts">
import { CocoChat, coco } from 'coco-gui';
import { onBeforeRouteUpdate, useRoute } from 'vue-router';

const route = useRoute();
let itm = coco.KnowledgeBase.getInstance().items.get(route.params.id as string);
onBeforeRouteUpdate((to, from) => { itm = coco.KnowledgeBase.getInstance().items.get(to.params.id as string); });
</script>