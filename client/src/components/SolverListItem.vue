<template>
  <v-list-item :value="solver.id" :title="solver.name" :prepend-icon="icon" />
</template>
  
<script setup>
import { computed } from 'vue';
import { Solver } from '@/solver';

const props = defineProps({
  solver: {
    type: Solver,
    required: true
  }
});

const icon = computed(() => {
  switch (props.solver.state) {
    case 'reasoning':
    case 'adapting': return 'mdi-brain';
    case 'idle': return 'mdi-pause-circle';
    case 'executing': return 'mdi-play-circle';
    case 'finished': return 'mdi-check-circle';
    case 'failed': return 'mdi-alert-circle';
    default: return null;
  }
});
</script>