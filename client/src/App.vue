<template>
  <coco-app>
    <template #header>
      <router-link to="/">
        <h1>CoCo</h1>
      </router-link>
    </template>
    <template #drawer>
      <n-menu v-model:value="active_key" :options="menu" accordion />
    </template>
    <router-view />
  </coco-app>
</template>

<script setup lang="ts">
import 'coco-gui/dist/style.css';
import { Box20Regular, Circle20Regular, BrainCircuit20Regular, PauseCircle20Regular, PlayCircle20Regular, CheckmarkCircle20Regular, ErrorCircle20Regular } from '@vicons/fluent';
import { NMenu, type MenuOption } from 'naive-ui';
import { CocoApp, taxonomy, rule, solver, coco } from 'coco-gui';
import { computed, h, ref } from 'vue';
import { RouterLink } from 'vue-router';

coco.KnowledgeBase.getInstance().connect();

const active_key = ref<string | null>(null);
const menu = computed<MenuOption[]>(() => [
  { key: 'home', label: () => h(RouterLink, { to: '/' }, { default: () => 'Home' }) },
  { key: 'types', label: 'Types', children: types_menu_options(coco.KnowledgeBase.getInstance().types) },
  { key: 'items', label: 'Items', children: items_menu_options(coco.KnowledgeBase.getInstance().items) },
  { key: 'reactive_rules', label: 'Reactive Rules', children: reactive_rules_menu_options(coco.KnowledgeBase.getInstance().reactive_rules) },
  { key: 'deliberative_rules', label: 'Deliberative Rules', children: deliberative_rules_menu_options(coco.KnowledgeBase.getInstance().deliberative_rules) },
  { key: 'solvers', label: 'Solvers', children: solvers_menu_options(coco.KnowledgeBase.getInstance().solvers) }
]);

function types_menu_options(types: Map<string, taxonomy.Type>): MenuOption[] {
  return Array.from(types.values()).map(type => {
    return {
      label: () => h(RouterLink, { to: { name: 'type', params: { id: type.id } } }, { default: () => type.name }),
      key: type.id,
      icon: () => h(Box20Regular),
    }
  });
}

function items_menu_options(items: Map<string, taxonomy.Item>): MenuOption[] {
  return Array.from(items.values()).map(item => {
    return {
      label: () => h(RouterLink, { to: { name: 'item', params: { id: item.id } } }, { default: () => item.name }),
      key: item.id,
      icon: () => h(Circle20Regular),
    }
  });
}

function reactive_rules_menu_options(rules: Map<string, rule.ReactiveRule>): MenuOption[] {
  return Array.from(rules.values()).map(rule => {
    return {
      label: () => h(RouterLink, { to: { name: 'rule', params: { id: rule.id } } }, { default: () => rule.name }),
      key: rule.id,
      icon: () => h(Box20Regular),
    }
  });
}

function deliberative_rules_menu_options(rules: Map<string, rule.DeliberativeRule>): MenuOption[] {
  return Array.from(rules.values()).map(rule => {
    return {
      label: () => h(RouterLink, { to: { name: 'rule', params: { id: rule.id } } }, { default: () => rule.name }),
      key: rule.id,
      icon: () => h(Box20Regular),
    }
  });
}

function solvers_menu_options(solvers: Map<number, solver.Solver>): MenuOption[] {
  return Array.from(solvers.values()).map(slv => {
    const icn = computed(() => {
      switch (slv.state) {
        case solver.State.reasoning:
        case solver.State.adapting: return BrainCircuit20Regular;
        case solver.State.idle: return PauseCircle20Regular;
        case solver.State.executing: return PlayCircle20Regular;
        case solver.State.finished: return CheckmarkCircle20Regular;
        case solver.State.failed: return ErrorCircle20Regular;
      }
    });
    return {
      label: () => h(RouterLink, { to: { name: 'solver', params: { id: slv.id } } }, { default: () => slv.name }),
      key: slv.id,
      icon: () => h(icn.value)
    }
  });
}
</script>