<template>
  <n-grid x-gap="12" y-gap="12" :cols="2">
    <n-grid-item>
      <coco-frame title="Taxonomy">
        <taxonomy-graph graph_id="taxonomy-graph" style="height: 400px;"/>
      </coco-frame>
    </n-grid-item>
    <n-grid-item>
      <coco-frame title="Users" style="height: 400px;">
        <item-table type_name="User" />
      </coco-frame>
    </n-grid-item>
  </n-grid>
</template>

<script setup lang="ts">
import { NGrid, NGridItem } from 'naive-ui';
import { CocoFrame, TaxonomyGraph, ItemTable } from 'coco-gui';
</script>
