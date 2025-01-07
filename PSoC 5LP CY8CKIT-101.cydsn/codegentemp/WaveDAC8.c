/*******************************************************************************
* File Name: WaveDAC8.c
* Version 2.10
*
* Description:
*  This file provides the source code for the 8-bit Waveform DAC 
*  (WaveDAC8) Component.
*
********************************************************************************
* Copyright 2013, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "WaveDAC8.h"

uint8  WaveDAC8_initVar = 0u;

const uint8 CYCODE WaveDAC8_wave1[WaveDAC8_WAVE1_LENGTH] = { 129u,129u,129u,128u,129u,130u,130u,130u,130u,128u,128u,127u,124u,122u,122u,125u,127u,128u,127u,127u,127u,129u,128u,126u,123u,119u,117u,117u,116u,117u,120u,127u,139u,149u,156u,156u,150u,143u,132u,118u,102u,93u,89u,98u,107u,114u,119u,125u,137u,153u,165u,170u,163u,152u,135u,113u,92u,73u,68u,78u,98u,122u,141u,156u,171u,185u,194u,191u,179u,160u,137u,106u,74u,48u,39u,54u,84u,117u,147u,167u,185u,195u,196u,189u,172u,152u,134u,112u,84u,60u,49u,60u,88u,117u,144u,163u,184u,200u,208u,200u,179u,154u,130u,104u,77u,48u,30u,35u,61u,98u,132u,159u,183u,201u,211u,209u,187u,158u,129u,100u,70u,39u,17u,19u,44u,82u,122u,154u,185u,207u,221u,220u,200u,169u,134u,99u,64u,30u,6u,9u,35u,78u,123u,160u,193u,219u,232u,231u,207u,173u,139u,104u,72u,39u,17u,20u,49u,95u,141u,178u,206u,225u,230u,222u,193u,156u,120u,90u,67u,43u,29u,36u,67u,114u,160u,194u,217u,231u,234u,223u,193u,154u,116u,86u,65u,44u,31u,36u,62u,104u,145u,174u,194u,206u,212u,209u,188u,161u,132u,108u,90u,66u,47u,41u,54u,85u,117u,141u,161u,174u,184u,187u,173u,151u,125u,103u,87u,64u,46u,41u,56u,90u,126u,152u,173u,187u,197u,198u,183u,159u,130u,105u,85u,60u,38u,29u,41u,76u,115u,148u,176u,197u,212u,219u,209u,187u,158u,131u,109u,80u,50u,32u,35u,64u,102u,135u,165u,187u,204u,216u,212u,195u,169u,144u,124u,98u,72u,52u,51u,76u,110u,138u,164u,183u,198u,208u,203u,187u,161u,136u,116u,92u,66u,46u,43u,65u,98u,129u,156u,174u,191u,203u,201u,187u,160u,134u,112u,86u,60u,34u,27u,47u,79u,113u,141u,162u,180u,194u,198u,189u,167u,142u,123u,99u,73u,46u,33u,49u,77u,108u,136u,154u,172u,186u,190u,182u,161u,138u,120u,99u,76u,52u,41u,55u,81u,114u,142u,163u,181u,197u,203u,196u,174u,151u,131u,109u,84u,57u,41u,51u,76u,108u,139u,161u,182u,199u,208u,206u,190u,168u,149u,128u,103u,73u,52u,52u,70u,100u,129u,153u,177u,198u,210u,211u,196u,172u,151u,129u,105u,74u,49u,47u,63u,93u,124u,149u,172u,191u,202u,203u,188u,165u,142u,118u,94u,65u,42u,38u,52u,78u,106u,134u,158u,178u,192u,196u,182u,159u,135u,115u,93u,64u,40u,35u,47u,73u,103u,131u,157u,178u,194u,201u,191u,172u,149u,127u,105u,78u,52u,44u,51u,75u,104u,131u,159u,180u,198u,208u,203u,186u,165u,143u,119u,91u,64u,51u,53u,75u,101u,127u,153u,175u,194u,205u,202u,187u,166u,144u,122u,91u,65u,49u,48u,66u,93u,119u,147u,170u,190u,203u,202u,188u,168u,146u,124u,94u,64u,45u,41u,56u,79u,106u,135u,161u,184u,201u,204u,195u,176u,155u,134u,104u,75u,52u,45u,57u,77u,104u,132u,157u,180u,196u,200u,192u,175u,155u,135u,107u,78u,56u,49u,59u,80u,107u,134u,159u,181u,198u,204u,198u,181u,161u,140u,113u,84u,61u,49u,55u,73u,99u,126u,152u,175u,193u,202u,199u,184u,165u,146u,120u,92u,66u,52u,54u,70u,91u,119u,143u,168u,186u,198u,197u,185u,169u,150u,124u,96u,69u,52u,53u,66u,87u,113u,139u,164u,185u,198u,200u,189u,173u,153u,129u,101u,72u,54u,50u,59u,80u,105u,131u,158u,180u,196u,201u,193u,179u,159u,135u,107u,77u,56u,49u,55u,74u,99u,124u,152u,175u,192u,199u,191u,178u,161u,139u,113u,81u,59u,51u,55u,73u,98u,123u,149u,172u,192u,199u,194u,182u,163u,142u,116u,85u,62u,52u,54u,70u,92u,118u,144u,167u,188u,198u,195u,185u,170u,148u,123u,93u,69u,55u,56u,70u,90u,115u,141u,164u,185u,196u,197u,186u,173u,154u,130u,100u,73u,58u,56u,68u,89u,111u,137u,161u,182u,198u,198u,192u,177u,159u,136u,107u,79u,63u,58u,66u,84u,106u,132u,155u,178u,194u,197u,191u,178u,162u,139u,110u,82u,63u,57u,64u,79u,102u,126u,150u,173u,189u,194u,189u,178u,161u,141u,113u,85u,65u,55u,60u,75u,96u,119u,142u,166u,185u,190u,187u,177u,163u,146u,119u,92u,72u,60u,64u,76u,93u,116u,138u,161u,178u,186u,185u,177u,165u,149u,124u,98u,76u,63u,63u,74u,92u,114u,134u,157u,175u,185u,185u,177u,167u,152u,129u,102u,79u,65u,63u,70u,87u,107u,127u,150u,170u,180u,182u,176u,168u,154u,133u,109u,85u,69u,65u,70u,85u,104u,123u,146u,165u,178u,181u,177u,169u,157u,138u,113u,89u,71u,66u,71u,85u,102u,122u,144u,164u,177u,182u,178u,171u,161u,142u,120u,95u,77u,71u,73u,85u,102u,119u,141u,159u,174u,180u,177u,172u,161u,145u,124u,99u,81u,73u,73u,85u,100u,116u,136u,155u,170u,178u,176u,171u,162u,147u,127u,104u,84u,74u,72u,82u,96u,112u,130u,150u,167u,174u,173u,169u,164u,150u,132u,108u,88u,77u,73u,80u,94u,108u,127u,146u,162u,171u,172u,169u,163u,152u,135u,113u,92u,79u,74u,80u,92u,107u,124u,142u,158u,169u,171u,170u,164u,154u,138u,117u,96u,81u,74u,78u,89u,102u,119u,137u,155u,166u,168u,168u,164u,155u,140u,119u,98u,82u,75u,78u,87u,100u,116u,133u,151u,163u,167u,167u,164u,157u,144u,124u,103u,87u,77u,79u,88u,98u,113u,130u,149u,163u,167u,169u,166u,159u,148u,129u,109u,91u,81u,81u,87u,98u,112u,129u,147u,161u,167u,170u,166u,163u,151u,134u,113u,95u,83u,82u,86u,95u,108u,124u,143u,158u,166u,169u,166u,163u,153u,136u,117u,97u,84u,81u,84u,93u,105u,120u,139u,155u,163u,167u,167u,163u,154u,139u,120u,100u,85u,80u,83u,90u,103u,118u,136u,153u,163u,167u,167u,164u,157u,144u,125u,106u,88u,83u,84u,92u,103u,116u,135u,151u,162u,168u,168u,166u,160u,147u,129u,110u,93u,85u,85u,91u,100u,114u,131u,150u,161u,166u,168u,166u,161u,149u,133u,112u,94u,85u,83u,89u,97u,109u,126u,145u,157u,164u,166u,166u,162u,150u,135u,115u,96u,85u,81u,86u,94u,106u,123u,140u,154u,162u,165u,165u,162u,152u,137u,117u,99u,87u,82u,87u,93u,103u,120u,137u,152u,162u,164u,166u,164u,155u,142u,123u,104u,89u,85u,88u,93u,103u,119u,136u,151u,161u,164u,166u,164u,157u,145u,126u,108u,94u,87u,89u,93u,102u,115u,133u,148u,159u,163u,166u,164u,159u,148u,130u,111u,96u,88u,89u,92u,100u,113u,130u,144u,156u,162u,165u,165u,160u,150u,134u,114u,98u,89u,89u,93u,99u,112u,126u,143u,154u,160u,164u,166u,161u,153u,137u,119u,103u,92u,91u,93u,98u,109u,123u,141u,153u,159u,163u,165u,162u,155u,141u,123u,105u,94u,92u,92u,98u,108u,122u,137u,149u,156u,162u,164u,162u,156u,144u,127u,108u,96u,93u,92u,96u,106u,119u,135u,147u,155u,160u,163u,163u,157u,145u,129u,112u,97u,94u,93u,96u,104u,116u,131u,145u,152u,158u,161u,161u,158u,147u,133u,115u,101u,94u,93u,96u,102u,113u,129u,142u,150u,156u,160u,161u,158u,149u,135u,118u,104u,98u,94u,96u,102u,112u,128u,141u,148u,156u,158u,160u,159u,152u,138u,121u,105u,99u,95u,97u,102u,110u,126u,139u,148u,154u,157u,160u,160u,154u,142u,125u,108u,101u,96u,97u,101u,109u,122u,136u,146u,152u,156u,159u,160u,155u,143u,127u,112u,102u,96u,96u,98u,107u,120u,133u,144u,151u,155u,159u,159u,155u,146u,130u,114u,103u,98u,97u,99u,105u,118u,131u,142u,150u,155u,158u,160u,156u,148u,133u,116u,105u,99u,96u,98u,102u,115u,129u,140u,149u,154u,160u,161u,158u,150u,136u,119u,107u,99u,97u,97u,102u,113u,126u,137u,147u,153u,158u,161u,160u,152u,138u,122u,108u,100u,96u,96u,99u,110u,122u,136u,146u,152u,158u,160u,160u,153u,141u,124u,111u,100u,97u,96u,98u,107u,120u,134u,144u,150u,157u,160u,159u,156u,143u,127u,112u,102u,97u,96u,96u,106u,119u,132u,142u,150u,156u,159u,160u,157u,146u,130u,116u,104u,97u,96u,96u,104u,115u,129u,140u,148u,154u,159u,160u,158u,148u,133u,118u,106u,99u,96u,95u,101u,113u,125u,137u,145u,153u,157u,160u,160u,151u,136u,121u,107u,99u,96u,94u,99u,110u,124u,135u,144u,152u,157u,161u,160u,153u,140u,124u,109u,100u,95u,93u,98u,107u,120u,133u,143u,151u,156u,160u,162u,154u,142u,125u,111u,102u,95u,93u,96u,105u,119u,132u,142u,150u,155u,160u,161u,155u,143u,128u,113u,103u,96u,93u,95u,103u,117u,129u,139u,149u,153u,159u,161u,157u,146u,131u,116u,105u,98u,95u,95u,102u,114u,126u,137u,146u,152u,157u,160u,158u,148u,134u,117u,107u,99u,94u,94u,100u,113u,123u,135u,143u,150u,155u,161u,159u,150u,137u,121u,110u,101u,96u,94u,98u,108u,122u,133u,142u,149u,156u,160u,160u,153u,139u,123u,112u,102u,95u,93u,96u,107u,118u,130u,142u,148u,154u,159u,160u,155u,142u,126u,114u,103u,98u,94u,95u,106u,117u,129u,139u,147u,154u,158u,160u,156u,144u,128u,116u,106u,99u,94u,96u,104u,116u,127u,137u,144u,151u,157u,159u,157u,147u,130u,118u,108u,100u,95u,95u,103u,113u,125u,135u,144u,150u,156u,160u,158u,148u,135u,121u,110u,101u,96u,95u,101u,110u,123u,134u,141u,149u,155u,159u,160u,151u,137u,124u,112u,104u,97u,95u,99u,108u,120u,131u,140u,147u,154u,160u,160u,153u,140u,126u,114u,105u,98u,95u,99u,107u,119u,129u,139u,147u,152u,158u,161u,154u,142u,128u,116u,107u,98u,94u,98u,106u,116u,128u,137u,145u,152u,157u,160u,156u,144u,131u,119u,109u,101u,96u,99u,105u,116u,126u,134u,143u,150u,157u,161u,157u,146u,133u,120u,111u,102u,97u,98u,104u,114u,125u,134u,140u,148u,155u,161u,158u,149u,135u,122u,113u,104u,98u,98u,102u,111u,123u,131u,140u,147u,153u,160u,159u,151u,139u,125u,116u,106u,99u,98u,101u,110u,121u,130u,138u,145u,153u,159u,160u,152u,140u,127u,118u,108u,101u,98u,100u,109u,118u,129u,136u,144u,151u,158u,160u,155u,143u,130u,119u,110u,102u,98u,99u,108u,117u,127u,136u,142u,150u,157u,160u,155u,144u,132u,121u,111u,103u,99u,99u,106u,115u,125u,134u,140u,148u,155u,159u,156u,147u,135u,124u,113u,105u,100u,99u,104u,114u,123u,131u,139u,146u,154u,159u,157u,148u,138u,126u,115u,106u,101u,98u,104u,111u,121u,130u,138u,145u,154u,160u,159u,150u,139u,128u,118u,108u,101u,97u,101u,109u,120u,129u,136u,145u,152u,158u,160u,152u,141u,130u,119u,109u,101u,98u,100u,107u,117u,127u,136u,143u,151u,157u,160u,153u,143u,132u,119u,110u,101u,97u,98u,106u,116u,126u,133u,141u,150u,157u,160u,155u,145u,134u,123u,111u,102u,98u,97u,104u,114u,125u,132u,140u,148u,157u,161u,156u,148u,135u,123u,113u,105u,98u,98u,102u,112u,122u,130u,138u,146u,156u,161u,158u,150u,139u,126u,116u,105u,99u,97u,100u,109u,119u,128u,137u,146u,155u,160u,158u,152u,142u,129u,118u,106u,99u,96u,98u,106u,117u,125u,135u,144u,154u,161u,160u,154u,143u,132u,120u,108u,97u,94u,95u,104u,114u,123u,133u,143u,153u,162u,163u,156u,146u,134u,122u,109u,99u,93u,94u,101u,111u,122u,132u,142u,153u,163u,165u,160u,150u,137u,124u,111u,99u,92u,92u,98u,109u,120u,130u,142u,152u,162u,165u,161u,153u,139u,125u,112u,99u,92u,91u,97u,106u,118u,128u,139u,151u,161u,166u,163u,154u,141u,127u,113u,100u,90u,88u,93u,102u,115u,126u,135u,150u,161u,167u,166u,159u,146u,132u,116u,103u,92u,87u,91u,100u,112u,124u,137u,149u,164u,171u,171u,165u,152u,137u,120u,104u,92u,86u,89u,96u,109u,123u,136u,151u,165u,174u,174u,167u,154u,138u,119u,103u,89u,82u,84u,93u,106u,123u,138u,154u,168u,176u,176u,167u,153u,134u,112u,93u,78u,71u,74u,85u,103u,122u,139u,157u,171u,179u,179u,170u,154u,133u,112u,92u,76u,69u,72u,84u,102u,123u,142u,161u,175u,184u,186u,176u,161u,141u,119u,100u,85u,78u,80u,90u,108u,128u,148u,166u,180u,188u,189u,180u,165u,145u,124u,104u,88u,79u,81u,90u,107u,126u,145u,163u,177u,185u,186u,178u,164u,145u,123u,102u,86u,75u,75u,82u,97u,116u,134u,153u,168u,179u,182u,176u,164u,146u,125u,105u,86u,75u,72u,78u,92u,110u,128u,149u,165u,177u,182u,179u,168u,150u,131u,111u,92u,79u,76u,79u,93u,111u,131u,152u,169u,182u,189u,187u,177u,162u,143u,123u,104u,91u,85u,88u,100u,119u,138u,157u,175u,189u,196u,195u,186u,172u,153u,133u,114u,101u,94u,96u,107u,122u,140u,158u,174u,187u,194u,193u,185u,171u,154u,135u,115u,102u,96u,97u,105u,118u,135u,150u,166u,179u,185u,185u,179u,166u,150u,132u,113u,99u,92u,91u,98u,110u,125u,141u,157u,169u,177u,178u,176u,163u,149u,132u,115u,101u,93u,90u,97u,108u,121u,138u,154u,168u,177u,180u,178u,169u,155u,138u,122u,107u,97u,95u,101u,111u,125u,142u,157u,171u,181u,185u,184u,176u,162u,147u,132u,118u,109u,106u,110u,121u,134u,149u,163u,176u,184u,187u,186u,177u,166u,153u,138u,125u,117u,114u,118u,127u,140u,153u,164u,174u,179u,181u,178u,170u,159u,147u,133u,123u,116u,113u,116u,122u,133u,143u,152u,160u,163u,165u,163u,157u,149u,139u,127u,119u,113u,111u,111u,117u,124u,132u,139u,146u,150u,153u,152u,149u,144u,137u,130u,123u,118u,115u,115u,120u,126u,132u,138u,143u,147u,150u,151u,149u,146u,141u,135u,130u,125u,124u,123u,125u,130u,136u,141u,147u,150u,153u,154u,154u,150u,145u,140u,134u,131u,128u,127u,128u,132u,135u,141u,145u,147u,150u,151u,149u,146u,143u,137u,133u,129u,125u,125u,125u,128u,131u,135u,138u,141u,142u,142u,141u,140u,136u,134u,131u,127u,127u,125u,126u,127u,131u,132u,134u,134u,135u,135u,135u,134u,132u,130u,129u };
const uint8 CYCODE WaveDAC8_wave2[WaveDAC8_WAVE2_LENGTH] = { 131u,129u,129u,127u,127u,127u,127u,126u,128u,128u,128u,127u,128u,129u,130u,132u,131u,129u,129u,129u,130u,131u,131u,132u,132u,132u,131u,130u,128u,128u,128u,127u,127u,125u,123u,122u,122u,128u,138u,149u,154u,156u,151u,147u,139u,126u,112u,102u,99u,102u,106u,111u,115u,115u,114u,118u,135u,158u,177u,177u,161u,143u,126u,109u,89u,72u,66u,72u,91u,113u,131u,138u,137u,141u,156u,183u,202u,192u,163u,130u,111u,98u,81u,64u,61u,76u,102u,131u,148u,151u,148u,146u,159u,182u,203u,198u,169u,137u,119u,110u,96u,78u,75u,89u,116u,140u,151u,145u,135u,129u,135u,157u,180u,177u,154u,127u,113u,107u,94u,78u,75u,88u,113u,137u,145u,133u,117u,107u,114u,137u,162u,168u,155u,134u,122u,118u,103u,83u,74u,83u,103u,123u,130u,124u,114u,109u,117u,141u,169u,179u,169u,149u,137u,131u,116u,95u,84u,91u,114u,140u,150u,146u,136u,129u,133u,153u,175u,182u,167u,141u,126u,119u,106u,88u,80u,92u,119u,152u,170u,169u,160u,149u,146u,157u,175u,179u,162u,132u,110u,102u,90u,73u,63u,71u,100u,135u,154u,154u,143u,128u,123u,137u,164u,178u,167u,139u,120u,114u,104u,86u,69u,66u,85u,113u,130u,129u,117u,101u,96u,111u,142u,164u,164u,145u,131u,129u,123u,107u,92u,89u,105u,132u,147u,146u,136u,121u,113u,120u,141u,159u,158u,138u,120u,119u,117u,109u,100u,99u,117u,147u,166u,168u,157u,139u,125u,127u,144u,160u,157u,134u,115u,111u,114u,110u,101u,101u,121u,150u,170u,171u,163u,145u,130u,129u,144u,162u,162u,139u,117u,111u,108u,100u,90u,87u,101u,129u,152u,157u,153u,140u,127u,127u,142u,162u,164u,141u,118u,109u,105u,96u,84u,76u,88u,115u,136u,147u,145u,133u,119u,117u,131u,150u,154u,136u,117u,108u,104u,98u,90u,84u,93u,115u,138u,149u,147u,133u,117u,112u,121u,139u,145u,130u,111u,105u,102u,99u,93u,90u,100u,125u,149u,162u,163u,154u,139u,130u,136u,154u,162u,148u,128u,114u,109u,106u,99u,94u,101u,122u,148u,164u,169u,162u,148u,141u,148u,164u,172u,160u,141u,127u,119u,112u,102u,93u,99u,119u,143u,159u,164u,158u,145u,136u,137u,152u,161u,151u,134u,121u,110u,101u,92u,85u,90u,105u,125u,142u,151u,146u,134u,121u,120u,136u,146u,141u,124u,109u,101u,93u,84u,76u,76u,91u,111u,129u,139u,136u,127u,116u,116u,132u,146u,144u,132u,120u,113u,108u,102u,95u,96u,108u,126u,143u,151u,149u,140u,126u,123u,135u,148u,147u,137u,128u,119u,117u,113u,106u,106u,118u,135u,151u,160u,158u,150u,136u,130u,137u,149u,149u,138u,127u,119u,114u,107u,99u,98u,108u,123u,139u,150u,152u,146u,134u,127u,137u,149u,150u,143u,133u,123u,117u,109u,98u,95u,102u,115u,128u,138u,142u,137u,128u,122u,129u,144u,148u,144u,137u,128u,122u,115u,107u,102u,107u,117u,129u,138u,141u,138u,125u,116u,123u,136u,141u,139u,132u,126u,122u,117u,109u,105u,109u,120u,133u,141u,145u,142u,131u,120u,122u,133u,139u,136u,130u,123u,118u,114u,106u,102u,104u,114u,127u,138u,144u,146u,136u,128u,130u,141u,146u,146u,139u,132u,125u,119u,111u,105u,105u,113u,125u,134u,143u,144u,138u,129u,132u,142u,150u,149u,144u,137u,134u,128u,119u,112u,110u,115u,125u,133u,140u,141u,134u,125u,124u,132u,139u,142u,138u,131u,128u,124u,116u,110u,107u,111u,119u,127u,133u,137u,131u,121u,121u,127u,135u,136u,134u,129u,125u,121u,114u,107u,104u,107u,114u,122u,130u,136u,130u,124u,124u,131u,139u,142u,139u,134u,130u,126u,120u,112u,109u,111u,118u,126u,133u,138u,137u,129u,127u,134u,141u,146u,142u,137u,133u,129u,124u,118u,112u,115u,120u,129u,136u,143u,141u,135u,132u,136u,143u,148u,146u,141u,136u,132u,126u,119u,114u,114u,119u,125u,133u,141u,141u,134u,131u,135u,141u,147u,144u,139u,135u,130u,125u,117u,111u,110u,116u,121u,128u,135u,136u,132u,128u,131u,138u,143u,142u,137u,133u,128u,121u,115u,107u,106u,110u,114u,123u,131u,133u,129u,125u,128u,135u,141u,143u,138u,134u,130u,125u,118u,110u,107u,110u,114u,123u,132u,134u,131u,127u,129u,135u,142u,143u,141u,136u,133u,128u,120u,114u,111u,112u,117u,124u,135u,138u,136u,132u,133u,138u,144u,145u,142u,139u,134u,128u,122u,114u,111u,113u,116u,123u,132u,138u,135u,131u,132u,137u,143u,145u,143u,139u,135u,130u,122u,115u,110u,110u,113u,118u,128u,133u,133u,129u,129u,135u,142u,143u,141u,138u,135u,130u,122u,114u,110u,109u,111u,117u,126u,132u,132u,128u,128u,134u,141u,143u,142u,138u,135u,131u,125u,115u,111u,110u,112u,117u,125u,133u,133u,130u,129u,133u,139u,143u,143u,141u,138u,134u,128u,120u,115u,112u,113u,118u,126u,134u,135u,131u,129u,133u,139u,144u,143u,141u,139u,135u,129u,120u,116u,113u,113u,117u,125u,133u,134u,131u,128u,132u,138u,142u,142u,141u,137u,135u,129u,121u,115u,113u,112u,116u,124u,131u,132u,130u,127u,129u,135u,139u,140u,138u,137u,134u,129u,121u,116u,112u,111u,114u,122u,129u,132u,129u,125u,127u,133u,138u,139u,138u,134u,133u,129u,121u,116u,113u,111u,115u,122u,130u,133u,130u,127u,128u,133u,138u,140u,139u,136u,136u,131u,124u,119u,115u,113u,115u,122u,131u,135u,134u,130u,129u,135u,139u,141u,140u,140u,138u,134u,127u,121u,118u,115u,116u,122u,131u,134u,134u,130u,131u,134u,138u,141u,141u,139u,138u,135u,128u,122u,117u,115u,114u,119u,127u,132u,132u,129u,128u,131u,136u,139u,138u,137u,137u,133u,128u,122u,117u,114u,113u,116u,125u,131u,131u,127u,126u,130u,134u,138u,139u,137u,136u,135u,128u,123u,119u,115u,115u,118u,125u,131u,133u,129u,128u,131u,134u,139u,139u,138u,139u,136u,130u,125u,121u,117u,115u,118u,125u,133u,134u,130u,129u,130u,135u,138u,139u,139u,138u,136u,131u,126u,121u,117u,115u,116u,124u,131u,132u,129u,128u,129u,132u,137u,139u,138u,138u,137u,131u,126u,122u,116u,114u,114u,121u,129u,130u,129u,127u,127u,131u,135u,137u,137u,137u,136u,132u,127u,123u,118u,114u,114u,119u,128u,132u,129u,127u,127u,130u,135u,137u,138u,137u,137u,133u,127u,123u,119u,116u,115u,120u,127u,130u,130u,128u,127u,129u,134u,138u,137u,138u,136u,133u,128u,125u,119u,116u,115u,119u,126u,131u,130u,127u,127u,129u,134u,137u,136u,137u,136u,134u,128u,124u,121u,116u,114u,117u,124u,130u,129u,127u,127u,130u,134u,136u,136u,137u,137u,133u,129u,126u,122u,116u,115u,116u,123u,129u,129u,128u,126u,128u,133u,135u,137u,137u,137u,134u,130u,126u,122u,117u,113u,115u,122u,128u,129u,128u,126u,128u,131u,134u,134u,136u,136u,134u,131u,128u,123u,118u,114u,115u,121u,126u,129u,128u,127u,127u,131u,133u,134u,136u,135u,135u,131u,128u,123u,119u,114u,115u,120u,127u,129u,128u,126u,126u,130u,133u,134u,135u,136u,134u,132u,128u,123u,119u,113u,114u,119u,126u,129u,128u,126u,126u,129u,132u,134u,135u,136u,135u,132u,128u,125u,120u,115u,114u,117u,125u,128u,128u,125u,126u,129u,132u,132u,135u,136u,135u,133u,129u,127u,121u,116u,113u,117u,123u,128u,128u,125u,126u,128u,132u,133u,136u,136u,136u,133u,129u,126u,121u,115u,113u,117u,124u,127u,128u,126u,126u,127u,131u,131u,134u,136u,135u,134u,130u,127u,121u,116u,114u,116u,122u,127u,128u,126u,125u,127u,130u,132u,134u,135u,135u,133u,130u,127u,123u,118u,113u,115u,121u,127u,127u,125u,124u,126u,130u,131u,134u,135u,137u,134u,132u,128u,124u,119u,115u,115u,120u,126u,128u,126u,125u,126u,128u,131u,133u,135u,137u,134u,131u,129u,125u,120u,114u,116u,119u,125u,127u,127u,125u,126u,128u,130u,133u,134u,135u,133u,132u,129u,127u,120u,116u,115u,118u,124u,128u,126u,125u,126u,128u,130u,132u,134u,136u,134u,131u,130u,126u,121u,117u,116u,119u,124u,128u,127u,126u,126u,128u,130u,133u,135u,135u,135u,133u,131u,127u,122u,117u,116u,118u,124u,128u,127u,126u,125u,127u,130u,132u,135u,136u,135u,134u,131u,128u,123u,117u,114u,118u,123u,127u,127u,126u,125u,127u,130u,132u,134u,136u,136u,134u,132u,129u,124u,118u,115u,117u,122u,128u,127u,125u,125u,126u,129u,132u,133u,136u,136u,134u,133u,130u,126u,119u,117u,116u,122u,127u,127u,126u,126u,127u,129u,132u,134u,137u,137u,135u,133u,130u,126u,121u,117u,116u,121u,126u,127u,126u,126u,127u,129u,132u,134u,137u,136u,135u,133u,132u,128u,122u,118u,118u,120u,125u,127u,127u,126u,127u,128u,130u,133u,137u,136u,136u,134u,133u,128u,122u,117u,116u,121u,125u,128u,126u,126u,126u,129u,130u,133u,137u,137u,137u,135u,134u,130u,124u,118u,117u,119u,124u,126u,127u,126u,127u,129u,130u,132u,136u,137u,136u,136u,134u,131u,125u,119u,117u,119u,124u,127u,126u,126u,127u,129u,130u,132u,136u,137u,136u,136u,135u,131u,126u,120u,117u,119u,123u,127u,127u,126u,127u,128u,130u,132u,135u,136u,137u,135u,134u,132u,127u,120u,117u,119u,124u,126u,127u,127u,127u,128u,130u,132u,134u,136u,136u,136u,135u,133u,128u,121u,118u,119u,123u,126u,126u,126u,127u,129u,129u,131u,135u,136u,137u,136u,135u,134u,130u,123u,119u,118u,123u,126u,127u,126u,126u,127u,128u,130u,133u,135u,136u,136u,137u,134u,130u,124u,118u,118u,121u,125u,127u,127u,126u,127u,128u,129u,133u,135u,136u,136u,136u,135u,131u,124u,118u,118u,121u,125u,127u,126u,126u,127u,128u,129u,132u,134u,135u,136u,136u,135u,132u,127u,120u,118u,120u,124u,126u,127u,127u,127u,128u,129u,132u,134u,135u,135u,136u,135u,132u,127u,121u,118u,119u,124u,126u,127u,127u,127u,127u,129u,132u,134u,134u,136u,136u,135u,133u,127u,121u,119u,119u,124u,127u,126u,127u,126u,127u,128u,130u,134u,135u,137u,136u,136u,134u,128u,122u,119u,120u,123u,125u,126u,127u,127u,127u,128u,130u,132u,135u,137u,135u,136u,134u,129u,124u,119u,119u,122u,125u,127u,127u,127u,126u,128u,130u,132u,135u,135u,136u,136u,136u,130u,124u,119u,118u,122u,125u,126u,127u,128u,127u,127u,130u,131u,134u,135u,135u,136u,135u,132u,125u,119u,118u,121u,124u,125u,127u,126u,126u,127u,129u,132u,134u,136u,136u,137u,135u,132u,126u,121u,118u,121u,122u,124u,126u,127u,126u,126u,129u,132u,135u,135u,136u,138u,136u,134u,126u,122u,118u,120u,123u,125u,126u,126u,126u,127u,128u,130u,134u,135u,136u,138u,137u,134u,128u,121u,118u,121u,123u,125u,125u,126u,126u,126u,127u,128u,132u,134u,136u,136u,137u,135u,129u,122u,119u,120u,123u,125u,127u,127u,127u,126u,126u,129u,132u,133u,135u,136u,137u,135u,130u,124u,120u,120u,122u,125u,125u,126u,127u,127u,127u,129u,131u,134u,135u,135u,136u,135u,131u,124u,119u,118u,122u,124u,126u,127u,127u,127u,127u,129u,132u,134u,135u,137u,138u,136u,131u,124u,119u,118u,121u,123u,127u,127u,126u,125u,127u,128u,132u,134u,136u,137u,139u,137u,133u,126u,119u,118u,119u,123u,125u,127u,127u,126u,126u,128u,130u,134u,135u,136u,139u,138u,135u,127u,121u,118u,120u,122u,125u,127u,127u,126u,126u,128u,130u,132u,135u,136u,139u,140u,136u,128u,122u,118u,121u,124u,125u,128u,127u,126u,126u,127u,130u,131u,133u,137u,139u,140u,137u,130u,122u,119u,119u,123u,127u,128u,128u,127u,126u,126u,128u,131u,132u,135u,138u,141u,138u,132u,123u,118u,119u,122u,126u,128u,130u,129u,127u,126u,128u,131u,133u,136u,140u,142u,140u,134u,124u,120u,119u,123u,128u,130u,130u,129u,127u,127u,129u,132u,133u,136u,139u,141u,139u,134u,125u,120u,120u,123u,126u,129u,130u,127u,126u,125u,127u,130u,134u,138u,140u,141u,138u,131u,124u,122u,123u,124u,127u,129u,128u,126u,124u,122u,124u,130u,136u,141u,143u,141u,136u,129u,123u,122u,125u,127u,130u,131u,130u,128u,126u,124u,125u,130u,139u,147u,149u,147u,142u,134u,128u,127u,132u,135u,136u,137u,136u,135u,130u,127u,127u,133u,141u,149u,151u,150u,144u,135u,128u,127u,128u,131u,134u,135u,134u,131u,127u,122u,122u,127u,135u,142u,145u,144u,138u,129u,121u,120u,122u,125u,127u,128u,128u,124u,121u,118u,118u,121u,128u,136u,141u,141u,135u,126u,120u,117u,120u,122u,125u,128u,128u,126u,122u,119u,118u,122u,127u,136u,141u,143u,141u,133u,126u,123u,124u,128u,132u,133u,134u,134u,132u,129u,127u,128u,133u,141u,148u,150u,147u,141u,133u,130u,131u,133u,135u,136u,138u,138u,135u,130u,127u,128u,133u,142u,148u,148u,144u,138u,131u,127u,128u,131u,132u,131u,131u,131u,128u,124u,119u,120u,124u,131u,138u,139u,136u,128u,119u,115u,116u,118u,120u,120u,119u,118u,115u,112u,109u,109u,112u,119u,126u,131u,128u,121u,113u,109u,108u,111u,114u,115u,114u,113u,112u,110u,107u,106u,111u,119u,127u,130u,129u,124u,118u,113u,114u,116u,118u,118u,119u,120u,119u,116u,115u,116u,120u,126u,133u,138u,138u,136u,129u,125u,126u,126u,127u,127u,127u,126u,125u,125u,122u,122u,126u,131u,136u,142u,143u,141u,134u,130u,128u,128u,127u,125u,123u,121u,119u,117u,115u,114u,117u,121u,126u,132u,134u,132u,126u,120u,117u,114u,112u,111u,108u,107u,106u,104u,102u,101u,104u,107u,112u,119u,124u,124u,120u,114u,112u,109u,107u,106u,104u,104u,104u,105u,105u,104u,106u,110u,114u,121u,127u,129u,126u,121u,117u,115u,114u,113u,113u,112u,114u,114u,115u,115u,118u,120u,125u,130u,134u,136u,135u,129u,126u,123u,122u,121u,121u,121u,120u,119u,118u,119u,119u,120u,124u,128u,132u,133u,131u,126u,121u,117u,114u,114u,113u,113u,111u,111u,111u,111u,111u,112u,115u,120u,123u,126u,126u,121u,118u,115u,113u,110u,110u,109u,109u,109u,109u,109u,110u,112u,114u,117u,122u };

static uint8  WaveDAC8_Wave1Chan;
static uint8  WaveDAC8_Wave2Chan;
static uint8  WaveDAC8_Wave1TD;
static uint8  WaveDAC8_Wave2TD;


/*******************************************************************************
* Function Name: WaveDAC8_Init
********************************************************************************
*
* Summary:
*  Initializes component with parameters set in the customizer.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void WaveDAC8_Init(void) 
{
	WaveDAC8_IDAC8_Init();
	WaveDAC8_IDAC8_SetSpeed(WaveDAC8_HIGHSPEED);
	WaveDAC8_IDAC8_SetRange(WaveDAC8_DAC_RANGE);

	#if(WaveDAC8_DAC_MODE == WaveDAC8_CURRENT_MODE)
		WaveDAC8_IDAC8_SetPolarity(WaveDAC8_DAC_POL);
	#endif /* WaveDAC8_DAC_MODE == WaveDAC8_CURRENT_MODE */

	#if(WaveDAC8_OUT_MODE == WaveDAC8_BUFFER_MODE)
	   WaveDAC8_BuffAmp_Init();
	#endif /* WaveDAC8_OUT_MODE == WaveDAC8_BUFFER_MODE */

	/* Get the TD Number for the DMA channel 1 and 2   */
	WaveDAC8_Wave1TD = CyDmaTdAllocate();
	WaveDAC8_Wave2TD = CyDmaTdAllocate();
	
	/* Initialize waveform pointers  */
	WaveDAC8_Wave1Setup(WaveDAC8_wave1, WaveDAC8_WAVE1_LENGTH) ;
	WaveDAC8_Wave2Setup(WaveDAC8_wave2, WaveDAC8_WAVE2_LENGTH) ;
	
	/* Initialize the internal clock if one present  */
	#if defined(WaveDAC8_DacClk_PHASE)
	   WaveDAC8_DacClk_SetPhase(WaveDAC8_CLK_PHASE_0nS);
	#endif /* defined(WaveDAC8_DacClk_PHASE) */
}


/*******************************************************************************
* Function Name: WaveDAC8_Enable
********************************************************************************
*  
* Summary: 
*  Enables the DAC block and DMA operation.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
*******************************************************************************/
void WaveDAC8_Enable(void) 
{
	WaveDAC8_IDAC8_Enable();

	#if(WaveDAC8_OUT_MODE == WaveDAC8_BUFFER_MODE)
	   WaveDAC8_BuffAmp_Enable();
	#endif /* WaveDAC8_OUT_MODE == WaveDAC8_BUFFER_MODE */

	/* 
	* Enable the channel. It is configured to remember the TD value so that
	* it can be restored from the place where it has been stopped.
	*/
	(void)CyDmaChEnable(WaveDAC8_Wave1Chan, 1u);
	(void)CyDmaChEnable(WaveDAC8_Wave2Chan, 1u);
	
	/* set the initial value */
	WaveDAC8_SetValue(0u);
	
	#if(WaveDAC8_CLOCK_SRC == WaveDAC8_CLOCK_INT)  	
	   WaveDAC8_DacClk_Start();
	#endif /* WaveDAC8_CLOCK_SRC == WaveDAC8_CLOCK_INT */
}


/*******************************************************************************
* Function Name: WaveDAC8_Start
********************************************************************************
*
* Summary:
*  The start function initializes the voltage DAC with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as 
*  executing the stop function.
*
* Parameters:  
*  None
*
* Return: 
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void WaveDAC8_Start(void) 
{
	/* If not Initialized then initialize all required hardware and software */
	if(WaveDAC8_initVar == 0u)
	{
		WaveDAC8_Init();
		WaveDAC8_initVar = 1u;
	}
	
	WaveDAC8_Enable();
}


/*******************************************************************************
* Function Name: WaveDAC8_StartEx
********************************************************************************
*
* Summary:
*  The StartEx function sets pointers and sizes for both waveforms
*  and then starts the component.
*
* Parameters:  
*   uint8 * wavePtr1:     Pointer to the waveform 1 array.
*   uint16  sampleSize1:  The amount of samples in the waveform 1.
*   uint8 * wavePtr2:     Pointer to the waveform 2 array.
*   uint16  sampleSize2:  The amount of samples in the waveform 2.
*
* Return: 
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void WaveDAC8_StartEx(const uint8 * wavePtr1, uint16 sampleSize1, const uint8 * wavePtr2, uint16 sampleSize2)

{
	WaveDAC8_Wave1Setup(wavePtr1, sampleSize1);
	WaveDAC8_Wave2Setup(wavePtr2, sampleSize2);
	WaveDAC8_Start();
}


/*******************************************************************************
* Function Name: WaveDAC8_Stop
********************************************************************************
*
* Summary:
*  Stops the clock (if internal), disables the DMA channels
*  and powers down the DAC.
*
* Parameters:  
*  None  
*
* Return: 
*  None
*
*******************************************************************************/
void WaveDAC8_Stop(void) 
{
	/* Turn off internal clock, if one present */
	#if(WaveDAC8_CLOCK_SRC == WaveDAC8_CLOCK_INT)  	
	   WaveDAC8_DacClk_Stop();
	#endif /* WaveDAC8_CLOCK_SRC == WaveDAC8_CLOCK_INT */
	
	/* Disble DMA channels */
	(void)CyDmaChDisable(WaveDAC8_Wave1Chan);
	(void)CyDmaChDisable(WaveDAC8_Wave2Chan);

	/* Disable power to DAC */
	WaveDAC8_IDAC8_Stop();
}


/*******************************************************************************
* Function Name: WaveDAC8_Wave1Setup
********************************************************************************
*
* Summary:
*  Sets pointer and size for waveform 1.                                    
*
* Parameters:  
*  uint8 * WavePtr:     Pointer to the waveform array.
*  uint16  SampleSize:  The amount of samples in the waveform.
*
* Return: 
*  None 
*
*******************************************************************************/
void WaveDAC8_Wave1Setup(const uint8 * wavePtr, uint16 sampleSize)

{
	#if (CY_PSOC3)
		uint16 memoryType; /* determining the source memory type */
		memoryType = (WaveDAC8_HI16FLASHPTR == HI16(wavePtr)) ? HI16(CYDEV_FLS_BASE) : HI16(CYDEV_SRAM_BASE);
		
		WaveDAC8_Wave1Chan = WaveDAC8_Wave1_DMA_DmaInitialize(
		WaveDAC8_Wave1_DMA_BYTES_PER_BURST, WaveDAC8_Wave1_DMA_REQUEST_PER_BURST,
		memoryType, HI16(CYDEV_PERIPH_BASE));
	#else /* PSoC 5 */
		WaveDAC8_Wave1Chan = WaveDAC8_Wave1_DMA_DmaInitialize(
		WaveDAC8_Wave1_DMA_BYTES_PER_BURST, WaveDAC8_Wave1_DMA_REQUEST_PER_BURST,
		HI16(wavePtr), HI16(WaveDAC8_DAC8__D));
	#endif /* CY_PSOC3 */
	
	/*
	* TD is looping on itself. 
    * Increment the source address, but not the destination address. 
	*/
	(void)CyDmaTdSetConfiguration(WaveDAC8_Wave1TD, sampleSize, WaveDAC8_Wave1TD, 
                                    (uint8)CY_DMA_TD_INC_SRC_ADR | (uint8)WaveDAC8_Wave1_DMA__TD_TERMOUT_EN); 
	
	/* Set the TD source and destination address */
	(void)CyDmaTdSetAddress(WaveDAC8_Wave1TD, LO16((uint32)wavePtr), LO16(WaveDAC8_DAC8__D));
	
	/* Associate the TD with the channel */
	(void)CyDmaChSetInitialTd(WaveDAC8_Wave1Chan, WaveDAC8_Wave1TD);
}


/*******************************************************************************
* Function Name: WaveDAC8_Wave2Setup
********************************************************************************
*
* Summary:
*  Sets pointer and size for waveform 2.                                    
*
* Parameters:  
*  uint8 * WavePtr:     Pointer to the waveform array.
*  uint16  SampleSize:  The amount of samples in the waveform.
*
* Return: 
*  None
*
*******************************************************************************/
void WaveDAC8_Wave2Setup(const uint8 * wavePtr, uint16 sampleSize)
 
{
	#if (CY_PSOC3)
		uint16 memoryType; /* determining the source memory type */
		memoryType = (WaveDAC8_HI16FLASHPTR == HI16(wavePtr)) ? HI16(CYDEV_FLS_BASE) : HI16(CYDEV_SRAM_BASE);
			
		WaveDAC8_Wave2Chan = WaveDAC8_Wave2_DMA_DmaInitialize(
		WaveDAC8_Wave2_DMA_BYTES_PER_BURST, WaveDAC8_Wave2_DMA_REQUEST_PER_BURST,
		memoryType, HI16(CYDEV_PERIPH_BASE));
	#else /* PSoC 5 */
		WaveDAC8_Wave2Chan = WaveDAC8_Wave2_DMA_DmaInitialize(
		WaveDAC8_Wave2_DMA_BYTES_PER_BURST, WaveDAC8_Wave2_DMA_REQUEST_PER_BURST,
		HI16(wavePtr), HI16(WaveDAC8_DAC8__D));
	#endif /* CY_PSOC3 */
	
	/*
	* TD is looping on itself. 
	* Increment the source address, but not the destination address. 
	*/
	(void)CyDmaTdSetConfiguration(WaveDAC8_Wave2TD, sampleSize, WaveDAC8_Wave2TD, 
                                    (uint8)CY_DMA_TD_INC_SRC_ADR | (uint8)WaveDAC8_Wave2_DMA__TD_TERMOUT_EN); 
	
	/* Set the TD source and destination address */
	(void)CyDmaTdSetAddress(WaveDAC8_Wave2TD, LO16((uint32)wavePtr), LO16(WaveDAC8_DAC8__D));
	
	/* Associate the TD with the channel */
	(void)CyDmaChSetInitialTd(WaveDAC8_Wave2Chan, WaveDAC8_Wave2TD);
}


/* [] END OF FILE */
