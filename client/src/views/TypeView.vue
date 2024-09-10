<template>
  <type v-if="tp" :type="tp" :key="tp.id" />
</template>

<script setup lang="ts">
import { Type, coco } from 'coco-gui';
import { onBeforeRouteUpdate, useRoute } from 'vue-router';

const route = useRoute();
let tp = coco.KnowledgeBase.getInstance().types.get(route.params.id as string);
onBeforeRouteUpdate((to, from) => { tp = coco.KnowledgeBase.getInstance().types.get(to.params.id as string); });
</script>