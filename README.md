# 📂 Fydelis GUI — Versão Antiga / Histórica

> ⏳ **Arquivo Histórico — Não é a versão atual**
> Esta é uma das primeiras tentativas de interface gráfica para o FydelisTechOS.
> **Mantida aqui apenas para fins de estudo e comparação de evolução.**
>
> FydelisTechos • Salvador — Bahia, Brasil

---

## 📌 Sobre Este Repositório

Este repositório contém uma **versão muito antiga** da interface gráfica do Fydelis. O código aqui representa o estágio inicial de experimentação com GTK4. **Não faz parte do sistema atual e não está em desenvolvimento ativo.**

> 💡 **Por que mantive aqui?**
> Para mostrar de onde viemos. Comparar com versões atuais ajuda a entender como o projeto evoluiu, o que aprendemos e por que tomamos certas decisões ao longo do caminho. O código antigo não é "lixo" — é **histórico de aprendizado.**

---

## ⚠️ Estado do Código

| Aspecto | Situação |
|---|---|
| ✅ Funciona? | Pode funcionar em ambiente antigo, mas sem garantias |
| 🔧 Mantido? | ❌ Não. Arquivado. |
| 📦 Dependências | GTK4 (versão da época — pode precisar de ajustes para compilar hoje) |
| 🎯 Propósito | 📚 Estudo histórico e comparação de evolução |

---

## 📖 O Que Esta Versão Nos Mostra

| Característica | Esta Versão Antiga | Versão Atual do Projeto |
|---|---|---|
| Estrutura do código | Tudo junto em poucos arquivos | Separado em módulos organizados |
| Integração com terminal | Muito básica, apenas exibição | FydelTerm completo e independente |
| Conhecimento de GTK4 | Básico, experimental | ✅ Compreendido e aplicado |
| Objetivo | Testar se era possível | Decidir se era a solução certa |

> 🌱 *"Todo projeto começa simples. O segredo é não ter vergonha da primeira versão — e sim documentar até onde ela chegou."*

---

## 🚀 Se Quiser Testar (Para Referência)

```bash
# Pode ser necessário ajustar versões de dependências antigas
sudo apt install libgtk-4-dev build-essential

make clean
make
./fydelis-gui
