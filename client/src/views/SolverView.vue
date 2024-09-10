<template>
  <solver v-if="slv" :solver="slv" :key="slv.id" />
</template>

<script setup lang="ts">
import { Solver, coco } from 'coco-gui';
import { onBeforeRouteUpdate, useRoute } from 'vue-router';

const route = useRoute();
let slv = coco.KnowledgeBase.getInstance().solvers.get(parseInt(route.params.id as string));
onBeforeRouteUpdate((to, from) => { slv = coco.KnowledgeBase.getInstance().solvers.get(parseInt(to.params.id as string)); });
</script>