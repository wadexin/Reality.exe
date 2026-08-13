# Reality.exe Third-Party Asset Register

## Purpose

This register records external source content that actually enters the Reality.exe repository. It is an operational provenance record, not legal advice. Each future acquisition must preserve the exact license and receipt evidence available on its acquisition date.

## Task 020 vertical slice — 2026-08-13

No third-party files entered the repository during the initial Task 020 implementation.

The recommended paid Fab packs were not present in the local Epic/Fab Vault, and Task 020 did not authorize purchasing them. No unrelated Vault content was substituted. The first vertical slice therefore uses only Reality.exe-owned Unreal assets created from Unreal Engine basic shape content already available to the project. No ambientCG texture was downloaded or imported; the first shared materials are procedural color/roughness/metallic materials without external texture inputs.

| Asset/source | Publisher | Source | Acquisition date | License | Original package | Imported Reality paths | Purpose | Modifications/derived assets | Notes |
|---|---|---|---|---|---|---|---|---|---|
| None for Task 020 initial pass | N/A | N/A | N/A | N/A | N/A | N/A | N/A | N/A | Paid-pack import portion deferred; no external source files are present. |

## Required fields for future entries

Every external source must add a row before its first production commit with:

- Exact pack/asset name and version
- Publisher or original author
- Canonical listing/source URL and listing ID
- Acquisition date, displayed price, currency, and receipt/order identifier location
- Exact license name, version, tier, and retained evidence location
- Original package name and declared Unreal compatibility
- Imported source paths and precise purpose
- Reality-owned derived paths, wrappers, instances, or modified duplicates
- Attribution, redistribution, repository-access, trademark, or embedded-source notes
- UE staging results, excluded dependencies, and later removal/replacement history

Receipts and account details belong in restricted team records, not in the repository.
