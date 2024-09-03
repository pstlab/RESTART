<template>
  <n-grid x-gap="12" y-gap="12" :cols="2">
    <n-grid-item>
      <coco-frame title="Taxonomy">
        <taxonomy-graph graph_id="taxonomy-graph" style="min-height: 400px;"></taxonomy-graph>
      </coco-frame>
    </n-grid-item>
  </n-grid>
</template>

<script setup lang="ts">
import { NGrid, NGridItem } from 'naive-ui';
import { CocoFrame, TaxonomyGraph } from 'coco-gui';
</script>
